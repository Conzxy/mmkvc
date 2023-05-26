#include "mmkv/mmkv.h"

#include "kanon/string/string_view.h"
#include "kanon/string/string_view_util.h"
#include "kanon/thread/count_down_latch.h"
#include "kanon/util/optional.h"

#include <iostream>
#include <stdio.h>
#include <string>

using namespace mmkv;
using namespace kanon;

kanon::optional<StringView> ExtractAfterEqual(kanon::StringView arg)
{
  kanon::StringView::size_type pos;
  if ((pos = arg.rfind('=')) != kanon::StringView::npos) {
    return make_optional(arg.substr(pos + 1));
  }
  return make_null_optional<StringView>();
}

kanon::optional<StringView> GetOptValue(kanon::StringView opt)
{
  return ExtractAfterEqual(opt);
}

void HandleInvalidOption(bool parse_result)
{
  if (!parse_result) {
    LOG_ERROR << "INVALID option format";
    LOG_ERROR << "Format: --option=[value]";
    exit(EXIT_FAILURE);
  }
}

void HandleInvalidInteger(bool parse_result)
{
  if (!parse_result) {
    LOG_ERROR << "Invalid integer";
    exit(EXIT_FAILURE);
  }
}

void SetInteger(StringView arg, u64 *p_ival)
{
  auto val_opt = GetOptValue(arg);
  HandleInvalidOption(val_opt);
  auto rn_opt = StringViewToU64(*val_opt);
  HandleInvalidInteger(rn_opt);
  *p_ival = *rn_opt;
}

int main(int argc, char *argv[])
{
  auto program_name = argv[0];
  --argc;
  ++argv;

  u64 request_num = 10000;
  u64 thread_num = 1;
  u64 client_num = 1000;

  for (int i = 0; i < argc; ++i) {
    StringView arg(argv[i]);
    if (arg.starts_with("--help") || arg.starts_with("-h")) {
      printf("Usage: %s [OPTIONS]\n"
             "[--request-num|-n]=integer\n"
             "[--thread|-t]=integer\n"
             "[--client|-c]=integer\n"
             "[--help|-h]\n",
             program_name);
      exit(EXIT_SUCCESS);
    } else if (arg.starts_with("--request-num") || arg.starts_with("-n")) {
      SetInteger(arg, &request_num);
    } else if (arg.starts_with("--client") || arg.starts_with("-c")) {
      SetInteger(arg, &client_num);
    } else if (arg.starts_with("--thread") || arg.starts_with("-t")) {
      SetInteger(arg, &thread_num);
    }
  }

  if (request_num < client_num) {
    LOG_ERROR << "The requst num must be >= client_num";
    exit(EXIT_FAILURE);
  }

  auto client_req_num = request_num / client_num;

  EventLoopThread loop_thr("MmkvClient");
  auto p_loop = loop_thr.StartRun();

  std::vector<std::unique_ptr<MmkvClient>> clis;
  clis.resize(client_num);
  for (size_t i = 0; i < client_num; ++i) {
    clis[i].reset(new MmkvClient(p_loop, "127.0.0.1:9998"));
  }

  kanon::TimeStamp start_time = TimeStamp::Now();
  u64 key = 0;
  kanon::CountDownLatch latch(request_num);

  for (size_t req_idx = 0; req_idx < client_req_num; ++req_idx) {
    for (size_t cli_idx = 0; cli_idx < client_num; ++cli_idx) {
      auto &cli = *clis[cli_idx];
      cli.AddStr(std::to_string(key), std::to_string(key),
                 [&latch](MmkvValue &value) {
                   latch.Countdown();
                 });
      ++key;
    }
  }
  latch.Wait();

  auto total_time = TimeStamp::Now() - start_time;
  std::cout << "Request total: " << key << '\n';
  std::cout << "Client total: " << client_num << '\n';
  std::cout << "Every client request: " << client_req_num << "\n";
  std::cout << "Total time: " << total_time.milliseconds() << '\n';
  std::cout << "QPS: "
            << double(request_num) /
                   (TimeStamp::Now() - start_time).microseconds() *
                   TimeStamp::kMicrosecondsPerSeconds_
            << '\n';

  for (auto &up_cli : clis) {
    up_cli->Close();
  }
}
