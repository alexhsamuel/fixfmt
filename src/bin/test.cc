#include <cstdlib>
#include <iostream>

#include "text.hh"

int
main(
  int argc,
  char* const* const argv)
{
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " LEN STRING\n";
    return 1;
  }

  size_t const length(atoi(argv[1]));
  string const str(argv[2]);

  std::cout << palide(str, length, ELLIPSIS, '~', 0.75) << "\n"
            << std::string(length, '=') << "\n";
  return 0;
}


