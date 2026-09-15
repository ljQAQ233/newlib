#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

int
tcgetattr(int fd, struct termios* tio)
{
  return ioctl(fd, TCGETS, tio);
}

int
tcsetattr(int fd, int act, const struct termios* tio)
{
  if (act < 0 || act > 2) {
    errno = EINVAL;
    return -1;
  }
  return ioctl(fd, TCSETS + act, tio);
}

int
tcsetpgrp(int fd, pid_t pgrp)
{
  return ioctl(fd, TIOCSPGRP, &pgrp);
}

int
tcsetwinsize(int fd, const struct winsize* wsz)
{
  return ioctl(fd, TIOCSWINSZ, wsz);
}

int
tcgetwinsize(int fd, struct winsize* wsz)
{
  return ioctl(fd, TIOCGWINSZ, wsz);
}

pid_t
tcgetpgrp(int fd)
{
  pid_t pgrp;
  if (ioctl(fd, TIOCGPGRP, &pgrp) < 0)
    return -1;
  return pgrp;
}

speed_t
cfgetospeed(const struct termios* tio)
{
  return tio->c_cflag & CBAUD;
}

speed_t
cfgetispeed(const struct termios* tio)
{
  return (tio->c_cflag & CIBAUD) / (CIBAUD / CBAUD);
}

#define PARENB 0000400

void
cfmakeraw(struct termios* t)
{
  t->c_iflag &=
    ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  t->c_oflag &= ~OPOST;
  t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  t->c_cflag &= ~(CSIZE | PARENB);
  t->c_cflag |= CS8;
  t->c_cc[VMIN] = 1;
  t->c_cc[VTIME] = 0;
}

int
cfsetospeed(struct termios* tio, speed_t speed)
{
  if (speed & ~CBAUD) {
    errno = EINVAL;
    return -1;
  }
  tio->c_cflag &= ~CBAUD;
  tio->c_cflag |= speed;
  return 0;
}

int
cfsetispeed(struct termios* tio, speed_t speed)
{
  if (speed & ~CBAUD) {
    errno = EINVAL;
    return -1;
  }
  tio->c_cflag &= ~CIBAUD;
  tio->c_cflag |= speed * (CIBAUD / CBAUD);
  return 0;
}

int
cfsetspeed(struct termios* tio, speed_t speed)
{
  int r = cfsetospeed(tio, speed);
  if (!r)
    cfsetispeed(tio, 0);
  return r;
}

int
tcdrain(int fd)
{
  return ioctl(fd, TCSBRK, 1);
}

int
tcflow(int fd, int action)
{
  return ioctl(fd, TCXONC, action);
}

int
tcflush(int fd, int queue)
{
  return ioctl(fd, TCFLSH, queue);
}

pid_t
tcgetsid(int fd)
{
  int sid;
  if (ioctl(fd, TIOCGSID, &sid) < 0)
    return -1;
  return sid;
}

int
tcsendbreak(int fd, int dur)
{
  return ioctl(fd, TCSBRK, 0);
}
