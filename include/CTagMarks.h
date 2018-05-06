#ifndef CTAG_MARKS_H
#define CTAG_MARKS_H

#include <vector>
#include <list>
#include <map>

class CTagMarkTag;
class CTagMarkMark;
class CBookmarkGroup;
class CFileBase;

class CTagMarkTagMgr {
 public:
  typedef std::map<std::string,CTagMarkTag  *> TagMap;
  typedef std::map<std::string,CTagMarkMark *> MarkMap;

 public:
  CTagMarkTagMgr();

  virtual ~CTagMarkTagMgr();

  virtual CTagMarkTag *createTag(const std::string &name, const std::string &desc="", uint rank=0);

  virtual CTagMarkMark *createMark(const std::string &url, const std::string &desc="", uint rank=0);

  CTagMarkTag *getDefaultTag();

  CTagMarkTag *addTag(const std::string &name, const std::string &desc="", uint rank=0);

  CTagMarkMark *addMark(const std::string &url, const std::string &desc="", uint rank=0);

  bool loadBookmarks(const std::string &filename);
  bool saveBookmarks(std::ostream &os);

  bool loadXML(const std::string &filename);
  bool saveXML(const std::string &filename);
  bool saveXML(CFileBase &file);

  void print(std::ostream &os) const;

  uint getNumTags() const { return tags_.size(); }

  TagMap::const_iterator tagsBegin() const { return tags_.begin(); }
  TagMap::const_iterator tagsEnd  () const { return tags_.end  (); }

  uint getNumMarks() const { return marks_.size(); }

  MarkMap::const_iterator marksBegin() const { return marks_.begin(); }
  MarkMap::const_iterator marksEnd  () const { return marks_.end  (); }

  void addMarksForTags(const std::vector<CTagMarkTag *> &tags,
                       std::vector<CTagMarkMark *> &marks) const;
  void addTagsForMarks(const std::vector<CTagMarkMark *> &marks,
                       std::vector<CTagMarkTag *> &tags) const;

 private:
  // no implementation to prevent copy
  CTagMarkTagMgr(const CTagMarkTagMgr &mgr);
  CTagMarkTagMgr &operator=(const CTagMarkTagMgr &mgr);

  void loadBookmarkGroup(CBookmarkGroup *group, int depth,
                         std::vector<CTagMarkTag *> &tags);

 private:
  TagMap       tags_;
  MarkMap      marks_;
  CTagMarkTag *default_tag_ { nullptr };
};

//------

class CTagMarkTag {
 public:
  typedef std::list<CTagMarkMark *>           MarkList;
  typedef std::map<std::string,CTagMarkTag *> TagMap;

 protected:
  friend class CTagMarkTagMgr;

  CTagMarkTag(CTagMarkTagMgr *mgr, const std::string &name,
              const std::string &desc="", uint rank=0);

  virtual ~CTagMarkTag() { }

 public:
  const std::string &getName() const { return name_; }
  const std::string &getDesc() const { return desc_; }
  uint               getRank() const { return rank_; }

  void setDesc(const std::string &desc) { desc_ = desc; }
  void setRank(uint rank) { rank_ = rank; }

  void addMark   (CTagMarkMark *mark);
  void removeMark(CTagMarkMark *mark);

  TagMap::const_iterator tagsBegin() const { initTags(); return tags_.begin(); }
  TagMap::const_iterator tagsEnd  () const { initTags(); return tags_.end  (); }

  uint getNumMarks() const { return marks_.size(); }

  MarkList::const_iterator marksBegin() const { return marks_.begin(); }
  MarkList::const_iterator marksEnd  () const { return marks_.end  (); }

 private:
  void initTags() const;

 private:
  CTagMarkTagMgr * mgr_        { nullptr };
  std::string      name_;
  std::string      desc_;
  uint             rank_       { 0 };
  MarkList         marks_;
  TagMap           tags_;
  bool             tags_valid_ { false };
};

//------

class CTagMarkMark {
 public:
  typedef std::map<std::string,CTagMarkTag *> TagMap;

 protected:
  friend class CTagMarkTagMgr;

  CTagMarkMark(CTagMarkTagMgr *mgr, const std::string &url,
               const std::string &desc="", uint rank=0);

  virtual ~CTagMarkMark() { }

 public:
  const std::string &getUrl () const { return url_ ; }
  const std::string &getDesc() const { return desc_; }
  uint               getRank() const { return rank_; }

  void setDesc(const std::string &desc) { desc_ = desc; }
  void setRank(uint rank) { rank_ = rank; }

  void addTag(const std::string &name, const std::string &desc="", uint rank=0);
  void addTag(CTagMarkTag *tag);

  void removeTag(const std::string &name);
  void removeTag(CTagMarkTag *tag);

  TagMap::const_iterator tagsBegin() const { return tags_.begin(); }
  TagMap::const_iterator tagsEnd  () const { return tags_.end  (); }

  bool hasTag(CTagMarkTag *tag) const;

 private:
  CTagMarkTagMgr* mgr_  { nullptr };
  std::string     url_;
  std::string     desc_;
  uint            rank_ { 0 };
  TagMap          tags_;
};

#endif
