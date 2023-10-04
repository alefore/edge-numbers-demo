#include <readline/history.h>
#include <readline/readline.h>

#include "src/concurrent/operation.h"
#include "src/concurrent/thread_pool.h"
#include "src/futures/futures.h"
#include "src/language/error/value_or_error.h"
#include "src/language/gc.h"
#include "src/language/overload.h"
#include "src/language/wstring.h"
#include "src/vm/default_environment.h"
#include "src/vm/value.h"
#include "src/vm/vm.h"

namespace gc = afc::language::gc;
namespace futures = afc::futures;

using afc::concurrent::OperationFactory;
using afc::concurrent::ThreadPool;
using afc::language::EmptyValue;
using afc::language::Error;
using afc::language::FromByteString;
using afc::language::MakeNonNullShared;
using afc::language::MakeNonNullUnique;
using afc::language::NonNull;
using afc::language::overload;
using afc::language::Success;
using afc::vm::Environment;
using afc::vm::Expression;
using afc::vm::NewDefaultEnvironment;
using afc::vm::Value;

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  auto thread_pool = MakeNonNullShared<ThreadPool>(64, nullptr);
  auto operation_factory = MakeNonNullShared<OperationFactory>(thread_pool);
  gc::Pool pool(gc::Pool::Options{
      .collect_duration_threshold = afc::infrastructure::Duration(0.02),
      .operation_factory = operation_factory.get_shared(),
      .max_bag_shards = 1});
  gc::Root<Environment> environment = pool.NewRoot(
      MakeNonNullUnique<Environment>(NewDefaultEnvironment(pool).ptr()));

  while (true) {
    std::unique_ptr<char, decltype(&free)> line(readline(">>> "), &free);
    if (!line) break;
    if (*line) add_history(line.get());
    std::visit(
        overload{[&pool, &environment](
                     NonNull<std::unique_ptr<Expression>> expression) -> void {
                   std::function<void()> resume_callback;
                   Evaluate(std::move(expression), pool, environment,
                            [&](std::function<void()> resume) {
                              resume_callback = std::move(resume);
                            })
                       .Transform([](const gc::Root<Value>& value) {
                         std::cout << value.ptr().value() << std::endl;
                         return futures::Past(Success());
                       })
                       .ConsumeErrors([](Error error) {
                         std::cerr << "Runtime error: " << error << std::endl;
                         return futures::Past(EmptyValue());
                       });
                   while (resume_callback != nullptr) {
                     std::function<void()> tmp = std::move(resume_callback);
                     resume_callback = nullptr;
                     tmp();
                     pool.Collect();
                   }
                 },
                 [](Error error) -> void {
                   std::cerr << "Compilation error: " << error << std::endl;
                 }},
        CompileString(FromByteString(line.get()), pool, environment));
    pool.Collect();
  }
  return 0;
}
