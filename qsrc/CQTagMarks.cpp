#include <CQTagMarks.h>
#include <CTagMarks.h>
#include <CFile.h>
#include <COSFile.h>
#include <CStrUtil.h>
#include <CMessage.h>

#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

#include <svg/left_svg.h>
#include <svg/right_svg.h>
#include <svg/up_svg.h>
#include <svg/down_svg.h>

#include <svg/save_svg.h>
#include <svg/save_www_svg.h>
#include <svg/go_svg.h>

#include <html_head_str.h>

//------

class CQPixmapButton : public QWidget {
 public:
  CQPixmapButton(const QPoint &p, int s, const QString &iconName) {
    icon_ = CQPixmapCacheInst->getIcon(iconName);

    rect_ = QRect(p.x(), p.y(), s, s);
  }

  void draw(QPainter *painter) {
    QPixmap pm = icon_.pixmap(rect_.size(), (active_ ? QIcon::Normal : QIcon::Disabled));

    painter->drawPixmap(rect_.topLeft(), pm);
  }

  bool updateActive(const QPoint &point) {
    bool active = active_;

    active_ = isInside(point);

    return (active != active_);
  }

  void setActive(bool active) {
    active_ = active;
  }

  bool isInside(const QPoint &point) {
    return rect_.contains(point);
  }

 private:
  bool  active_ { false };
  QIcon icon_;
  QRect rect_;
};

//------

class CQTagMarkTagMgr;

class CQTagMarkTag : public CTagMarkTag {
 public:
  CQTagMarkTag(CQTagMarkTagMgr *mgr, const std::string &name,
               const std::string &desc="", unsigned int rank=0);

  bool getValid() const;
  bool getVisible() const { return visible_; }

  void draw(QPainter *painter, const QPoint &pos);

  QString getText();

  QSize getSize();

  bool updateActive(const QPoint &point) {
    bool active = active_;

    active_ = isInside(point);

    return (active != active_);
  }

  bool isInside(const QPoint &point);

  bool getActive() const { return active_; }

  void setActive(bool active) { active_ = active; }

 private:
  CQTagMarkTagMgr *mgr_     { nullptr };
  QRect            rect_;
  bool             active_  { false };
  bool             visible_ { false };
};

//-----

class CQTagMarkMark : public CTagMarkMark {
 public:
  CQTagMarkMark(CQTagMarkTagMgr *mgr, const std::string &url,
                const std::string &desc="", unsigned int rank=0);

  bool getValid() const;
  bool getVisible() const { return visible_; }

  void draw(QPainter *painter, const QPoint &pos);

  QString getText();

  QSize getSize();

  bool updateActive(const QPoint &point) {
    bool is_go;

    bool active = active_;

    active_ = isInside(point, &is_go);

    return (active != active_);
  }

  bool isInside(const QPoint &point, bool *is_go);

  bool getActive() const { return active_; }

  void setActive(bool active) { active_ = active; }

 private:
  CQTagMarkTagMgr *mgr_     { nullptr };
  QRect            rect_;
  bool             active_  { false };
  bool             visible_ { false };
  QPixmap          goPixmap_;
};

//------

class CQTagMarkTagMgr : public CTagMarkTagMgr {
 public:
  typedef std::vector<CTagMarkTag *>  Tags;
  typedef std::vector<CTagMarkMark *> Marks;

 public:
  CQTagMarkTagMgr(CQTagMarks *tagMarks);
 ~CQTagMarkTagMgr();

  CQTagMarks* tagMarks() const { return tagMarks_; }

  CQTagMarkTag *createTag(const std::string &name, const std::string &desc="",
                          unsigned int rank=0) {
    return new CQTagMarkTag(this, name, desc, rank);
  }

  CQTagMarkMark *createMark(const std::string &url, const std::string &desc="",
                            unsigned int rank=0) {
    return new CQTagMarkMark(this, url, desc, rank);
  }

  void drawTags (QPainter *painter);
  void drawMarks(QPainter *painter);

  void pressTag (const QPoint &pos);
  void pressMark(const QPoint &pos);

  void moveTag (const QPoint &pos);
  void moveMark(const QPoint &pos);

  void wwwSave();

  bool tagStackEmpty();

  void addTagToStack(const std::string &tagName);
  void addTagToStack(CQTagMarkTag *tag);

  const Tags &getTagStack() const { return undo_tag_stack_; }

  void prevTag();
  void nextTag();

  void updateTagStackLabel();

  void scrollTagsUp   (unsigned int d=1);
  void scrollTagsDown (unsigned int d=1);
  void scrollMarksUp  (unsigned int d=1);
  void scrollMarksDown(unsigned int d=1);

  void loadMark(CQTagMarkMark *mark, bool is_go);

  const Tags  &getTags();
  const Marks &getMarks();

 private:
  typedef std::map<std::string,CTagMarkTag *> NamedTags;

  CQTagMarks* tagMarks_    { nullptr };
  int         tag_offset_  { 0 };
  int         mark_offset_ { 0 };
  Tags        undo_tag_stack_;
  Tags        redo_tag_stack_;
  NamedTags   tag_stack_map_;
  Marks       marks_;
  bool        marks_valid_ { false };
  Tags        tags_;
  bool        tags_valid_ { false };
};

//------

struct CQTagsCompare {
  bool operator()(CTagMarkTag *tag1, CTagMarkTag *tag2) {
    int cmp = tag1->getRank() - tag2->getRank();

    if (cmp != 0)
      return (cmp > 0);
    else
      return (tag1->getName() < tag2->getName());
  }
};

struct CQMarksCompare {
  bool operator()(CTagMarkMark *mark1, CTagMarkMark *mark2) {
    int cmp = mark1->getRank() - mark2->getRank();

    if (cmp != 0)
      return (cmp > 0);
    else
      return (mark1->getUrl() < mark2->getUrl());
  }
};

//------

CQTagMarks::
CQTagMarks()
{
  font_ = QFont("Helvetica", 12);

  //------

  QVBoxLayout *layout = new QVBoxLayout(this);

  //------

  QHBoxLayout *slayout = new QHBoxLayout;

  layout->addLayout(slayout);

  mgr_ = new CQTagMarkTagMgr(this);

  mgr_->loadXML("tagmarks.xml");

  splitter_ = new QSplitter(this);

  slayout->addWidget(splitter_);

  //------

  tags_  = new CQTagMarksTags (this);
  marks_ = new CQTagMarksMarks(this);

  splitter_->addWidget(tags_);
  splitter_->addWidget(marks_);

  //------

  QWidget *control = new QWidget(this);

  splitter_->addWidget(control);

  QVBoxLayout *clayout = new QVBoxLayout(control);

  //------

  QPushButton *save_button = new QPushButton;

  save_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  save_button->setIcon(CQPixmapCacheInst->getIcon("SAVE"));
  save_button->setToolTip("Save As XML");

  connect(save_button, SIGNAL(clicked()), this, SLOT(saveClicked()));

  clayout->addWidget(save_button);

  //------

  QPushButton *www_save_button = new QPushButton;

  www_save_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  www_save_button->setIcon(CQPixmapCacheInst->getIcon("SAVE_WWW"));
  www_save_button->setToolTip("Save As HTML");

  connect(www_save_button, SIGNAL(clicked()), this, SLOT(wwwSaveClicked()));

  clayout->addWidget(www_save_button);

  //------

  QHBoxLayout *dlayout = new QHBoxLayout();

  clayout->addLayout(dlayout);

  QLabel *desc_label = new QLabel("Desc");

  desc_edit_ = new QLineEdit();

  connect(desc_edit_, SIGNAL(editingFinished()), this, SLOT(updateDesc()));

  dlayout->addWidget(desc_label);
  dlayout->addWidget(desc_edit_);

  //------

  QHBoxLayout *playout = new QHBoxLayout();

  clayout->addLayout(playout);

  QLabel *rank_label = new QLabel("Rank");

  rank_edit_ = new QLineEdit();

  connect(rank_edit_, SIGNAL(editingFinished()), this, SLOT(updateRank()));

  playout->addWidget(rank_label);
  playout->addWidget(rank_edit_);

  //------

  tag_list_widget_ = new QListWidget;

  clayout->addWidget(tag_list_widget_);

  clayout->addStretch();

  //------

  tag_stack_label_ = new QLabel("Tag Stack:");

  tag_stack_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout->addWidget(tag_stack_label_);

  //------

  message_ = new CMessage("CQTagMarks");

  QTimer::singleShot(1000, this, SLOT(checkMessage()));
}

CQTagMarks::
~CQTagMarks()
{
  delete message_;
}

int
CQTagMarks::
iconSize() const
{
  QFontMetrics fm(font());

  return fm.height() + 2;
}

int
CQTagMarks::
lineHeight() const
{
  QFontMetrics fm(font());

  return fm.height() + 2;
}

void
CQTagMarks::
loadMark(CQTagMarkMark *mark, bool is_go)
{
  mark_ = mark;

  desc_edit_->setText(QString(mark->getDesc().c_str()));
  rank_edit_->setText(QString("%1").arg(mark->getRank()));

  tag_list_widget_->clear();

  for (auto pt = mark->tagsBegin(); pt != mark->tagsEnd(); ++pt) {
    CTagMarkTag *tag = (*pt).second;

    tag_list_widget_->addItem(tag->getName().c_str());
  }

  if (is_go)
    system(("firefox " + mark_->getUrl() + " &").c_str());
}

void
CQTagMarks::
setTagStackLabel(const QString &label)
{
  tag_stack_label_->setText(label);
}

void
CQTagMarks::
redraw()
{
  update();
}

void
CQTagMarks::
saveClicked()
{
  mgr_->saveXML("tagmarks.xml");
}

void
CQTagMarks::
wwwSaveClicked()
{
  mgr_->wwwSave();
}

void
CQTagMarks::
updateDesc()
{
  if (mark_)
    mark_->setDesc(desc_edit_->text().toStdString());
}

void
CQTagMarks::
updateRank()
{
  if (mark_)
    mark_->setRank(CStrUtil::toInteger(rank_edit_->text().toStdString()));
}

void
CQTagMarks::
checkMessage()
{
  std::string msg;

  if (message_->recvClientMessage(msg)) {
    if      (msg == "..")
      mgr_->prevTag();
    else if (msg == "exit") {
      delete message_;

      message_ = nullptr;

      exit(0);
    }
    else
      mgr_->addTagToStack(msg);

    message_->sendServerMessage("ok");
  }

  QTimer::singleShot(1000, this, SLOT(checkMessage()));
}

//--------------

CQTagMarksTags::
CQTagMarksTags(CQTagMarks *marks) :
 marks_(marks)
{
  setMouseTracking(true);
  setFocusPolicy(Qt::WheelFocus);

  int s = marks->iconSize();
  int b = marks->border();

  int x = 2*b;

  left_pixmap_  = new CQPixmapButton(QPoint(x, b), s, "LEFT" ); x += s + b;
  right_pixmap_ = new CQPixmapButton(QPoint(x, b), s, "RIGHT"); x += s + b;
  up_pixmap_    = new CQPixmapButton(QPoint(x, b), s, "UP"   ); x += s + b;
  down_pixmap_  = new CQPixmapButton(QPoint(x, b), s, "DOWN" );
}

CQTagMarksTags::
~CQTagMarksTags()
{
  delete left_pixmap_;
  delete right_pixmap_;
  delete up_pixmap_;
  delete down_pixmap_;
}

void
CQTagMarksTags::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  marks_->getMgr()->drawTags(&painter);

  //---

  QColor bg = palette().color(QPalette::Normal, QPalette::Window);

  bg.setAlpha(200);

  int s = marks_->iconSize();

  painter.fillRect(QRect(0, 0, width(), s), bg);

  left_pixmap_ ->draw(&painter);
  right_pixmap_->draw(&painter);
  up_pixmap_   ->draw(&painter);
  down_pixmap_ ->draw(&painter);
}

void
CQTagMarksTags::
mousePressEvent(QMouseEvent *event)
{
  QPoint pos = event->pos();

  if      (left_pixmap_ ->isInside(pos))
    marks_->getMgr()->prevTag();
  else if (right_pixmap_->isInside(pos))
    marks_->getMgr()->nextTag();
  else if (up_pixmap_   ->isInside(pos))
    marks_->getMgr()->scrollTagsUp  ();
  else if (down_pixmap_ ->isInside(pos))
    marks_->getMgr()->scrollTagsDown();
  else
    marks_->getMgr()->pressTag(pos);
}

void
CQTagMarksTags::
mouseMoveEvent(QMouseEvent *event)
{
  QPoint pos = event->pos();

  bool update = false;

  if (left_pixmap_->updateActive(pos) || right_pixmap_->updateActive(pos) ||
      up_pixmap_  ->updateActive(pos) || down_pixmap_ ->updateActive(pos))
    update = true;

  if (! update)
    marks_->getMgr()->moveTag(pos);
  else
    marks_->redraw();
}

void
CQTagMarksTags::
wheelEvent(QWheelEvent *event)
{
  int num = abs(event->delta())/15;

  if (event->delta() > 0)
    marks_->getMgr()->scrollTagsUp(num);
  else
    marks_->getMgr()->scrollTagsDown(num);

  marks_->redraw();
}

//--------------

CQTagMarksMarks::
CQTagMarksMarks(CQTagMarks *marks) :
 marks_(marks)
{
  setMouseTracking(true);
  setFocusPolicy(Qt::WheelFocus);

  int s = marks->iconSize();
  int b = marks->border();

  int x = 2*b;

  up_pixmap_   = new CQPixmapButton(QPoint(x, b), s, "UP"  ); x += s + b;
  down_pixmap_ = new CQPixmapButton(QPoint(x, b), s, "DOWN");
}

CQTagMarksMarks::
~CQTagMarksMarks()
{
  delete up_pixmap_;
  delete down_pixmap_;
}

void
CQTagMarksMarks::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  marks_->getMgr()->drawMarks(&painter);

  //---

  QColor bg = palette().color(QPalette::Normal, QPalette::Window);

  bg.setAlpha(200);

  int s = marks_->iconSize();

  painter.fillRect(QRect(0, 0, width(), s), bg);

  up_pixmap_  ->draw(&painter);
  down_pixmap_->draw(&painter);
}

void
CQTagMarksMarks::
mousePressEvent(QMouseEvent *event)
{
  QPoint pos = event->pos();

  if      (up_pixmap_   ->isInside(pos))
    marks_->getMgr()->scrollMarksUp  ();
  else if (down_pixmap_ ->isInside(pos))
    marks_->getMgr()->scrollMarksDown();
  else
    marks_->getMgr()->pressMark(pos);
}

void
CQTagMarksMarks::
mouseMoveEvent(QMouseEvent *event)
{
  QPoint pos = event->pos();

  bool update = false;

  if (up_pixmap_->updateActive(pos) || down_pixmap_->updateActive(pos))
    update = true;

  if (! update)
    marks_->getMgr()->moveMark(event->pos());
  else
    marks_->redraw();
}

void
CQTagMarksMarks::
wheelEvent(QWheelEvent *event)
{
  int num = abs(event->delta())/15;

  if (event->delta() > 0)
    marks_->getMgr()->scrollMarksUp(num);
  else
    marks_->getMgr()->scrollMarksDown(num);

  marks_->redraw();
}

//--------------

CQTagMarkTagMgr::
CQTagMarkTagMgr(CQTagMarks *tagMarks) :
 CTagMarkTagMgr(), tagMarks_(tagMarks), tag_offset_(0), mark_offset_(0),
 marks_valid_(false), tags_valid_(false)
{
}

CQTagMarkTagMgr::
~CQTagMarkTagMgr()
{
}

void
CQTagMarkTagMgr::
drawTags(QPainter *painter)
{
  int lineHeight = tagMarks_->lineHeight();

  int x = tagMarks_->border();
  int y = tagMarks_->iconSize() - tag_offset_;

  const Tags &tags = getTags();

  for (const auto &tag : tags) {
    CQTagMarkTag *qtag = dynamic_cast<CQTagMarkTag *>(tag);

    if (! qtag || ! qtag->getValid()) continue;

    QPoint pos(x, y + lineHeight);

    qtag->draw(painter, pos);

    y += lineHeight;
  }
}

void
CQTagMarkTagMgr::
drawMarks(QPainter *painter)
{
  int lineHeight = tagMarks_->lineHeight();

  int x = tagMarks_->border();
  int y = tagMarks_->iconSize() - mark_offset_;

  const Marks &marks = getMarks();

  for (const auto &mark : marks) {
    CQTagMarkMark *qmark = dynamic_cast<CQTagMarkMark *>(mark);

    if (! qmark || ! qmark->getValid()) continue;

    QPoint pos(x, y + lineHeight);

    qmark->draw(painter, pos);

    y += lineHeight;
  }
}

void
CQTagMarkTagMgr::
pressTag(const QPoint &pos)
{
  const Tags &tags = getTags();

  for (const auto &tag : tags) {
    CQTagMarkTag *qtag = dynamic_cast<CQTagMarkTag *>(tag);

    if (! qtag || ! qtag->getVisible())
      continue;

    if (qtag->isInside(pos)) {
      addTagToStack(qtag);
      break;
    }
  }
}

void
CQTagMarkTagMgr::
pressMark(const QPoint &pos)
{
  bool is_go;

  const Marks &marks = getMarks();

  for (const auto &mark : marks) {
    CQTagMarkMark *qmark = dynamic_cast<CQTagMarkMark *>(mark);

    if (! qmark || ! qmark->getVisible())
      continue;

    if (qmark->isInside(pos, &is_go))
      loadMark(qmark, is_go);
  }
}

void
CQTagMarkTagMgr::
moveTag(const QPoint &pos)
{
  bool update = false;

  const Tags &tags = getTags();

  for (const auto &tag : tags) {
    CQTagMarkTag *qtag = dynamic_cast<CQTagMarkTag *>(tag);

    if (! qtag || ! qtag->getVisible())
      continue;

    if (qtag->updateActive(pos))
      update = true;
  }

  if (update)
    tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
moveMark(const QPoint &pos)
{
  bool is_go;

  bool update = false;

  const Marks &marks = getMarks();

  for (const auto &mark : marks) {
    CQTagMarkMark *qmark = dynamic_cast<CQTagMarkMark *>(mark);

    if (! qmark || ! qmark->getVisible())
      continue;

    bool active = qmark->isInside(pos, &is_go);

    if (active != qmark->getActive()) {
      qmark->setActive(active);

      update = true;
    }
  }

  if (update)
    tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
wwwSave()
{
  CFile file("temp.html");

  file.open(CFileBase::Mode::WRITE);

  file.printf("%s", html_head_str);

  file.printf("<div id=\"container\">\n");

  file.printf("<div id=\"center\" class=\"column\">\n");

  file.printf("<dl>\n");

  const Marks &marks = getMarks();

  for (const auto &mark : marks) {
    CQTagMarkMark *qmark = dynamic_cast<CQTagMarkMark *>(mark);

    file.printf("<dt><a href=\"%s\">%s</a></dt>\n",
                qmark->getUrl().c_str(), qmark->getDesc().c_str());
  }

  file.printf("</dl>\n");

  file.printf("</div>\n");

  file.printf("<div id=\"left\" class=\"column\">\n");

  file.printf("<dl>\n");

  const Tags &tags = getTags();

  for (const auto &tag : tags) {
    CQTagMarkTag *qtag = dynamic_cast<CQTagMarkTag *>(tag);

    file.printf("<dt>%s</dt>\n", qtag->getName().c_str());
  }

  file.printf("</dl>\n");

  file.printf("</div>\n");

  file.printf("</div>\n");

  file.printf("</body>\n");
  file.printf("</html>\n");

  QString cmd = QString("firefox file://%1/temp.html &").arg(COSFile::getCurrentDir().c_str());

  system(cmd.toStdString().c_str());
}

const CQTagMarkTagMgr::Tags &
CQTagMarkTagMgr::
getTags()
{
  if (! tags_valid_) {
    tags_.clear();

    if (tagStackEmpty()) {
      for (auto pt = tagsBegin(); pt != tagsEnd(); ++pt) {
        CTagMarkTag *tag = (*pt).second;

        tags_.push_back(tag);
      }
    }
    else {
      Marks marks;

      addMarksForTags(getTagStack(), marks);

      addTagsForMarks(marks, tags_);
    }

    sort(tags_.begin(), tags_.end(), CQTagsCompare());

    tags_valid_ = true;
  }

  return tags_;
}

const CQTagMarkTagMgr::Marks &
CQTagMarkTagMgr::
getMarks()
{
  if (! marks_valid_) {
    marks_.clear();

    if (tagStackEmpty() ) {
      for (auto pm = marksBegin(); pm != marksEnd(); ++pm) {
        CTagMarkMark *mark = (*pm).second;

        marks_.push_back(mark);
      }
    }
    else
      addMarksForTags(getTagStack(), marks_);

    sort(marks_.begin(), marks_.end(), CQMarksCompare());

    marks_valid_ = true;
  }

  return marks_;
}

bool
CQTagMarkTagMgr::
tagStackEmpty()
{
  return undo_tag_stack_.empty();
}

void
CQTagMarkTagMgr::
addTagToStack(const std::string &tagName)
{
  CTagMarkTag *tag = addTag(tagName);

  addTagToStack(dynamic_cast<CQTagMarkTag *>(tag));
}

void
CQTagMarkTagMgr::
addTagToStack(CQTagMarkTag *tag)
{
  if (! tag) return;

  const std::string &name = tag->getName();

  if (tag_stack_map_.find(name) != tag_stack_map_.end())
    return;

  tag_stack_map_[name] = tag;

  redo_tag_stack_.clear();

  undo_tag_stack_.push_back(tag);

  tag_offset_ = 0;

  updateTagStackLabel();

  marks_valid_ = false;
  tags_valid_  = false;

  tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
prevTag()
{
  if (! undo_tag_stack_.empty()) {
    CTagMarkTag *tag = undo_tag_stack_.back();

    undo_tag_stack_.pop_back();

    const std::string &name = tag->getName();

    tag_stack_map_.erase(name);

    redo_tag_stack_.push_back(tag);

    updateTagStackLabel();

    marks_valid_ = false;
    tags_valid_  = false;

    tagMarks_->redraw();
  }
}

void
CQTagMarkTagMgr::
nextTag()
{
  if (! redo_tag_stack_.empty()) {
    CTagMarkTag *tag = redo_tag_stack_.back();

    redo_tag_stack_.pop_back();

    undo_tag_stack_.push_back(tag);

    const std::string &name = tag->getName();

    tag_stack_map_[name] = tag;

    updateTagStackLabel();

    marks_valid_ = false;
    tags_valid_  = false;

    tagMarks_->redraw();
  }
}

void
CQTagMarkTagMgr::
updateTagStackLabel()
{
  QString str = "Tag Stack:";

  for (const auto &tag : undo_tag_stack_)
    str += QString(" ") + QString(tag->getName().c_str());

  tagMarks_->setTagStackLabel(str);
}

void
CQTagMarkTagMgr::
scrollTagsUp(unsigned int d)
{
  int lineHeight = tagMarks_->lineHeight();

  tag_offset_ -= lineHeight*d;

  if (tag_offset_ < 0)
    tag_offset_ = 0;

  tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
scrollTagsDown(unsigned int d)
{
  int lineHeight = tagMarks_->lineHeight();

  tag_offset_ += lineHeight*d;

  tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
scrollMarksUp(unsigned int d)
{
  int lineHeight = tagMarks_->lineHeight();

  mark_offset_ -= lineHeight*d;

  if (mark_offset_ < 0)
    mark_offset_ = 0;

  tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
scrollMarksDown(unsigned int d)
{
  int lineHeight = tagMarks_->lineHeight();

  mark_offset_ += lineHeight*d;

  tagMarks_->redraw();
}

void
CQTagMarkTagMgr::
loadMark(CQTagMarkMark *mark, bool is_go)
{
  tagMarks_->loadMark(mark, is_go);
}

//-----------

CQTagMarkTag::
CQTagMarkTag(CQTagMarkTagMgr *mgr, const std::string &name,
             const std::string &desc, unsigned int rank) :
 CTagMarkTag(mgr, name, desc, rank), mgr_(mgr)
{
}

bool
CQTagMarkTag::
getValid() const
{
  return (getName().length() && getNumMarks());
}

void
CQTagMarkTag::
draw(QPainter *painter, const QPoint &pos)
{
  CQTagMarks *tagMarks = mgr_->tagMarks();

  QFont font = tagMarks->font();

  QFontMetrics fm(font);

  int lineHeight = tagMarks->lineHeight();

  if (pos.y() > -lineHeight && pos.y() < painter->window().height()) {
    painter->setFont(font);

    QString text = getText();

    QColor color = getActive() ? QColor(255,0,0) : QColor(0,0,0);

    QBrush brush;

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);

    painter->setPen  (color);
    painter->setBrush(brush);

    painter->drawText(pos + QPoint(0, -fm.descent()), text);

    QSize size = getSize();

    QPoint pos1 = pos + QPoint(0, -size.height());

    rect_.setTopLeft(pos1);
    rect_.setSize   (size);

    visible_ = true;
  }
  else {
    QSize size(1, lineHeight);

    QPoint pos1 = pos + QPoint(0, -lineHeight);

    rect_.setTopLeft(pos1);
    rect_.setSize   (size);

    visible_ = false;
  }
}

QString
CQTagMarkTag::
getText()
{
  //return QString("%1 (%2)").arg(getName().c_str()).arg(getNumMarks());
  return QString(getName().c_str());
}

QSize
CQTagMarkTag::
getSize()
{
  QFont font = mgr_->tagMarks()->font();

  QFontMetrics fm(font);

  QString text = getText();

  return QSize(fm.width(text), fm.height());
}

bool
CQTagMarkTag::
isInside(const QPoint &point)
{
  return rect_.contains(point);
}

//----------

CQTagMarkMark::
CQTagMarkMark(CQTagMarkTagMgr *mgr, const std::string &url,
              const std::string &desc, unsigned int rank) :
 CTagMarkMark(mgr, url, desc, rank), mgr_(mgr)
{
  QIcon icon = CQPixmapCacheInst->getIcon("GO");

  int s = mgr_->tagMarks()->iconSize();

  goPixmap_ = icon.pixmap(QSize(s, s), QIcon::Normal);
}

bool
CQTagMarkMark::
getValid() const
{
  return getUrl().length();
}

void
CQTagMarkMark::
draw(QPainter *painter, const QPoint &pos)
{
  CQTagMarks *tagMarks = mgr_->tagMarks();

  QFont font = tagMarks->font();

  QFontMetrics fm(font);

  int lineHeight = tagMarks->lineHeight();

  if (pos.y() > -lineHeight && pos.y() < painter->window().height()) {
    painter->setFont(font);

    QString text = getText();

    QColor color = getActive() ? QColor(255,0,0) : QColor(0,0,0);

    QBrush brush;

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);

    painter->setPen  (color);
    painter->setBrush(brush);

    painter->drawText(pos + QPoint(0, -fm.descent()), text);

    QSize size = getSize();

    if (getActive()) {
      int w = fm.width(text) + 4;

      QPoint pos1 = pos + QPoint(w, -lineHeight/2 - goPixmap_.height()/2);

      painter->drawPixmap(pos1, goPixmap_);

      size += QSize(20, 0);
    }

    QPoint pos1 = pos + QPoint(0, -size.height());

    rect_.setTopLeft(pos1);
    rect_.setSize   (size);

    visible_ = true;
  }
  else {
    QSize size(1, lineHeight);

    QPoint pos1 = pos + QPoint(0, -lineHeight);

    rect_.setTopLeft(pos1);
    rect_.setSize   (size);

    visible_ = false;
  }
}

QString
CQTagMarkMark::
getText()
{
  return QString(getUrl().c_str());
}

QSize
CQTagMarkMark::
getSize()
{
  CQTagMarks *tagMarks = mgr_->tagMarks();

  QFont font = tagMarks->font();

  QFontMetrics fm(font);

  int lineHeight = tagMarks->lineHeight();

  QString text = getText();

  return QSize(fm.width(text), lineHeight);
}

bool
CQTagMarkMark::
isInside(const QPoint &point, bool *is_go)
{
  bool inside = rect_.contains(point);

  *is_go = (rect_.right() - point.x() < 20);

  return inside;
}
