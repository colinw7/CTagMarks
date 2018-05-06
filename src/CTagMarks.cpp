#include <CTagMarks.h>
#include <CBookmarksLib.h>
#include <CXMLLib.h>
#include <CStrUtil.h>
#include <CAutoPtr.h>

CTagMarkTagMgr::
CTagMarkTagMgr()
{
  default_tag_ = createTag("", "");
}

CTagMarkTagMgr::
~CTagMarkTagMgr()
{
}

CTagMarkTag *
CTagMarkTagMgr::
createTag(const std::string &name, const std::string &desc, uint rank)
{
  return new CTagMarkTag(this, name, desc, rank);
}

CTagMarkMark *
CTagMarkTagMgr::
createMark(const std::string &url, const std::string &desc, uint rank)
{
  return new CTagMarkMark(this, url, desc, rank);
}

CTagMarkTag *
CTagMarkTagMgr::
getDefaultTag()
{
  return default_tag_;
}

CTagMarkTag *
CTagMarkTagMgr::
addTag(const std::string &name, const std::string &desc, uint rank)
{
  auto p = tags_.find(name);

  if (p != tags_.end())
    return (*p).second;

  CTagMarkTag *tag = createTag(name, desc, rank);

  tags_[name] = tag;

  return tag;
}

CTagMarkMark *
CTagMarkTagMgr::
addMark(const std::string &url, const std::string &desc, uint rank)
{
  auto p = marks_.find(url);

  if (p != marks_.end())
    return (*p).second;

  CTagMarkMark *mark = createMark(url, desc, rank);

  marks_[url] = mark;

  return mark;
}

bool
CTagMarkTagMgr::
loadBookmarks(const std::string &filename)
{
  CBookmarkFile bookmarks(filename);

  bookmarks.setIgnoreDuplicates(true);

  bookmarks.read();

  CBookmarkGroup *top = bookmarks.getGroup();

  if (! top)
    return false;

  int depth = 1;

  std::vector<CTagMarkTag *> tags;

  loadBookmarkGroup(top, depth, tags);

  return true;
}

void
CTagMarkTagMgr::
loadBookmarkGroup(CBookmarkGroup *group, int depth,
                  std::vector<CTagMarkTag *> &tags)
{
  if (group->getName() == "Bookmarks Toolbar Folder")
    return;

  std::vector<CTagMarkTag *> tags1(tags);

  CTagMarkTag *tag = addTag(group->getName(), "", 100 - 10*depth);

  tags1.push_back(tag);

  CBookmarkGroup::group_iterator pg1 = group->groupsBegin();
  CBookmarkGroup::group_iterator pg2 = group->groupsEnd  ();

  for ( ; pg1 != pg2; ++pg1)
    loadBookmarkGroup(*pg1, depth + 1, tags1);

  CBookmarkGroup::bookmark_iterator pb1 = group->bookmarksBegin();
  CBookmarkGroup::bookmark_iterator pb2 = group->bookmarksEnd  ();

  for ( ; pb1 != pb2; ++pb1) {
    CTagMarkMark *mark = addMark((*pb1)->getUrl(), (*pb1)->getName());

    std::vector<CTagMarkTag *>::iterator pt1 = tags1.begin();
    std::vector<CTagMarkTag *>::iterator pt2 = tags1.end  ();

    for ( ; pt1 != pt2; ++pt1)
      mark->addTag(*pt1);
  }
}

bool
CTagMarkTagMgr::
saveBookmarks(std::ostream &)
{
  return false;
}

bool
CTagMarkTagMgr::
loadXML(const std::string &filename)
{
  CXML xml;

  CXMLTag *tag1;

  if (! xml.read(filename, &tag1))
    return false;

  CAutoPtr<CXMLTag> tag(tag1);

  if (tag->getName() != "tagmarks")
    return false;

  CXMLTag::TokenArray tokens = tag->getChildren();

  for (const auto &token : tokens) {
    if (! token->isTag()) continue;

    CXMLTag *tag = token->getTag();

    if      (tag->getName() == "tag") {
      std::string name, desc;

      uint rank = 0;

      const CXMLTag::OptionArray &options = tag->getOptions();

      CXMLTag::OptionArray::const_iterator po1 = options.begin();
      CXMLTag::OptionArray::const_iterator po2 = options.end  ();

      for ( ; po1 != po2; ++po1) {
        CXMLTagOption *option = *po1;

        if      (option->getName() == "name")
          name = option->getValue();
        else if (option->getName() == "desc")
          desc = option->getValue();
        else if (option->getName() == "rank")
          rank = CStrUtil::toInteger(option->getValue());
      }

      if (name != "")
        addTag(name, desc, rank);
    }
    else if (tag->getName() == "mark") {
      std::string url, desc;

      uint rank = 0;

      const CXMLTag::OptionArray &options = tag->getOptions();

      CXMLTag::OptionArray::const_iterator po1 = options.begin();
      CXMLTag::OptionArray::const_iterator po2 = options.end  ();

      for ( ; po1 != po2; ++po1) {
        CXMLTagOption *option = *po1;

        if      (option->getName() == "url")
          url  = option->getValue();
        else if (option->getName() == "desc")
          desc = option->getValue();
        else if (option->getName() == "rank")
          rank = CStrUtil::toInteger(option->getValue());
      }

      CTagMarkMark *mark = addMark(url, desc, rank);

      CXMLTag::TokenArray tokens = tag->getChildren();

      CXMLTag::TokenArray::const_iterator pt1 = tokens.begin();
      CXMLTag::TokenArray::const_iterator pt2 = tokens.end  ();

      for ( ; pt1 != pt2; ++pt1) {
        CXMLToken *token = *pt1;

        if (! token->isTag()) continue;

        CXMLTag *tag = token->getTag();

        if (tag->getName() != "tag") continue;

        std::string name, desc;

        uint rank = 0;

        const CXMLTag::OptionArray &options = tag->getOptions();

        CXMLTag::OptionArray::const_iterator po1 = options.begin();
        CXMLTag::OptionArray::const_iterator po2 = options.end  ();

        for ( ; po1 != po2; ++po1) {
          CXMLTagOption *option = *po1;

          if      (option->getName() == "name")
            name = option->getValue();
          else if (option->getName() == "desc")
            desc = option->getValue();
          else if (option->getName() == "rank")
            rank = CStrUtil::toInteger(option->getValue());
        }

        if (name != "") {
          CTagMarkTag *tag = addTag(name, desc, rank);

          mark->addTag(tag);
        }
      }
    }
  }

  return true;
}

bool
CTagMarkTagMgr::
saveXML(const std::string &filename)
{
  CFile file(filename);

  if (! file.open(CFile::Mode::WRITE))
    return false;

  return saveXML(file);
}

bool
CTagMarkTagMgr::
saveXML(CFileBase &file)
{
  file.printf("<tagmarks>\n");

  TagMap::const_iterator pt1 = tagsBegin();
  TagMap::const_iterator pt2 = tagsEnd  ();

  for ( ; pt1 != pt2; ++pt1) {
    CTagMarkTag *tag = (*pt1).second;

    std::string name = CXMLNamedCharMgrInst->encodeString(tag->getName());
    std::string desc = CXMLNamedCharMgrInst->encodeString(tag->getDesc());
    uint        rank = tag->getRank();

    if (tag->getName() == "") continue;

    file.printf("<tag name='%s' desc='%s' rank='%d'/>\n",
                name.c_str(), desc.c_str(), rank);
  }

  MarkMap::const_iterator pm1 = marksBegin();
  MarkMap::const_iterator pm2 = marksEnd  ();

  for ( ; pm1 != pm2; ++pm1) {
    CTagMarkMark *mark = (*pm1).second;

    std::string url  = CXMLNamedCharMgrInst->encodeString(mark->getUrl ());
    std::string desc = CXMLNamedCharMgrInst->encodeString(mark->getDesc());
    uint        rank = mark->getRank();

    file.printf("<mark url='%s' desc='%s' rank='%d'>\n",
                url.c_str(), desc.c_str(), rank);

    CTagMarkMark::TagMap::const_iterator pt1 = mark->tagsBegin();
    CTagMarkMark::TagMap::const_iterator pt2 = mark->tagsEnd  ();

    for ( ; pt1 != pt2; ++pt1) {
      CTagMarkTag *tag = (*pt1).second;

      if (tag->getName() == "") continue;

      std::string name = CXMLNamedCharMgrInst->encodeString(tag->getName());

      file.printf("<tag name='%s'/>\n", name.c_str());
    }

    file.printf("</mark>\n");
  }

  file.printf("</tagmarks>\n");

  return true;
}

void
CTagMarkTagMgr::
print(std::ostream &os) const
{
  os << "Tags: ";

  TagMap::const_iterator pt1 = tags_.begin();
  TagMap::const_iterator pt2 = tags_.end  ();

  for (char first = true; pt1 != pt2; ++pt1, first = false) {
    CTagMarkTag *tag = (*pt1).second;

    if (! first) os << ", ";

    os << "'" << tag->getName() << "'";
  }

  os << std::endl;
}

void
CTagMarkTagMgr::
addMarksForTags(const std::vector<CTagMarkTag *> &tags,
                std::vector<CTagMarkMark *> &marks) const
{
  MarkMap::const_iterator pm1 = marksBegin();
  MarkMap::const_iterator pm2 = marksEnd  ();

  for ( ; pm1 != pm2; ++pm1) {
    CTagMarkMark *mark = (*pm1).second;

    std::vector<CTagMarkTag *>::const_iterator pt1 = tags.begin();
    std::vector<CTagMarkTag *>::const_iterator pt2 = tags.end  ();

    bool found = true;

    for ( ; pt1 != pt2; ++pt1) {
      if (! mark->hasTag(*pt1)) {
        found = false;
        break;
      }
    }

    if (found)
      marks.push_back(mark);
  }
}

void
CTagMarkTagMgr::
addTagsForMarks(const std::vector<CTagMarkMark *> &marks,
                std::vector<CTagMarkTag *> &tags) const
{
  std::map<std::string,CTagMarkTag *> tag_map;

  std::vector<CTagMarkMark *>::const_iterator pm1 = marks.begin();
  std::vector<CTagMarkMark *>::const_iterator pm2 = marks.end  ();

  for ( ; pm1 != pm2; ++pm1) {
    CTagMarkMark *mark = *pm1;

    CTagMarkMark::TagMap::const_iterator pt1 = mark->tagsBegin();
    CTagMarkMark::TagMap::const_iterator pt2 = mark->tagsEnd  ();

    for ( ; pt1 != pt2; ++pt1) {
      CTagMarkTag *tag = (*pt1).second;

      const std::string &name = tag->getName();

      if (tag_map.find(name) == tag_map.end()) {
        tag_map[name] = tag;

        tags.push_back(tag);
      }
    }
  }
}

//-----------------

CTagMarkTag::
CTagMarkTag(CTagMarkTagMgr *mgr, const std::string &name,
            const std::string &desc, uint rank) :
 mgr_(mgr), name_(name), desc_(desc), rank_(rank), tags_valid_(false)
{
}

void
CTagMarkTag::
addMark(CTagMarkMark *mark)
{
  marks_.push_back(mark);

  tags_valid_ = false;
}

void
CTagMarkTag::
removeMark(CTagMarkMark *mark)
{
  marks_.remove(mark);

  tags_valid_ = false;
}

void
CTagMarkTag::
initTags() const
{
  if (tags_valid_)
    return;

  CTagMarkTag *th = const_cast<CTagMarkTag *>(this);

  th->tags_.clear();

  MarkList::const_iterator pm1 = marksBegin();
  MarkList::const_iterator pm2 = marksEnd  ();

  for ( ; pm1 != pm2; ++pm1) {
    CTagMarkMark *mark = *pm1;

    CTagMarkMark::TagMap::const_iterator pt1 = mark->tagsBegin();
    CTagMarkMark::TagMap::const_iterator pt2 = mark->tagsEnd  ();

    for ( ; pt1 != pt2; ++pt1) {
      CTagMarkTag *tag = (*pt1).second;

      th->tags_[tag->getName()] = tag;
    }
  }
}

//-----------------

CTagMarkMark::
CTagMarkMark(CTagMarkTagMgr *mgr, const std::string &url,
             const std::string &desc, uint rank) :
 mgr_(mgr), url_(url), desc_(desc), rank_(rank)
{
  CTagMarkTag *default_tag = mgr_->getDefaultTag();

  default_tag->addMark(this);
}

void
CTagMarkMark::
addTag(const std::string &name, const std::string &desc, uint rank)
{
  CTagMarkTag *tag = mgr_->addTag(name, desc, rank);

  addTag(tag);
}

void
CTagMarkMark::
addTag(CTagMarkTag *tag)
{
  if (tags_.empty()) {
    CTagMarkTag *default_tag = mgr_->getDefaultTag();

    default_tag->removeMark(this);
  }

  tag->addMark(this);

  tags_[tag->getName()] = tag;
}

void
CTagMarkMark::
removeTag(const std::string &name)
{
  CTagMarkTag *tag = mgr_->addTag(name);

  removeTag(tag);
}

void
CTagMarkMark::
removeTag(CTagMarkTag *tag)
{
  tag->removeMark(this);

  tags_.erase(tag->getName());

  if (tags_.empty()) {
    CTagMarkTag *default_tag = mgr_->getDefaultTag();

    default_tag->addMark(this);
  }
}

bool
CTagMarkMark::
hasTag(CTagMarkTag *tag) const
{
  return (tags_.find(tag->getName()) != tags_.end());
}
