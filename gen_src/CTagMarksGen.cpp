#include <CTagMarks.h>
#include <COStreamFile.h>

int
main(int argc, char **argv)
{
  CTagMarkTagMgr mgr;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-')
      continue;

    mgr.loadBookmarks(argv[i]);
  }

  COStreamFile file(std::cout);

  mgr.saveXML(file);

  return 0;
}
