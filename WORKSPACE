load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

http_archive(
    name = "edge",
    sha256 = "fc6fe2ca0e726790a4756feda89ac2d8cf2a153392cdcab592986093dafb44cb",
    strip_prefix = "edge-0.8",
    urls = ["https://github.com/alefore/edge/archive/refs/tags/v0.8.tar.gz"],
    build_file = "@//:edge.BUILD",
)

#new_local_repository(
#    name = "edge",
#    path = "/home/alejo/edge-clang/edge",
#    build_file = "@//:edge.BUILD",
#)

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-0.6.0",
    urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
)
