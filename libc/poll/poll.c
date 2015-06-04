#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout) {
  (void)(fds);
  (void)(nfds);
  (void)(timeout);
  return 0; // TODO
}
