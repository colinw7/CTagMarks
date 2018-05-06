#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QWheelEvent;
class QSplitter;
class QLineEdit;
class QLineEdit;
class QLabel;
class QListWidget;
class QTimer;

class CQTagMarkTagMgr;
class CQTagMarksTags;
class CQTagMarkMark;
class CQTagMarksMarks;
class CQPixmapButton;

class CMessage;

class CQTagMarks : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QFont font   READ font   WRITE setFont  )
  Q_PROPERTY(int   border READ border WRITE setBorder)

 public:
  CQTagMarks();
 ~CQTagMarks();

  CQTagMarkTagMgr *getMgr() const { return mgr_; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &v) { font_ = v; }

  int border() const { return border_; }
  void setBorder(int i) { border_ = i; }

  int iconSize() const;
  int lineHeight() const;

  void loadMark(CQTagMarkMark *mark, bool is_go);

  void setTagStackLabel(const QString &label);

  void redraw();

 private slots:
  void saveClicked();
  void wwwSaveClicked();
  void updateDesc();
  void updateRank();
  void checkMessage();

 private:
  CQTagMarkTagMgr* mgr_             { nullptr };
  QFont            font_;
  int              border_          { 4 };
  QSplitter*       splitter_        { nullptr };
  CQTagMarksTags*  tags_            { nullptr };
  CQTagMarksMarks* marks_           { nullptr };
  QLineEdit*       desc_edit_       { nullptr };
  QLineEdit*       rank_edit_       { nullptr };
  QListWidget*     tag_list_widget_ { nullptr };
  QLabel*          tag_stack_label_ { nullptr };
  CQTagMarkMark*   mark_            { nullptr };
  CMessage*        message_         { nullptr };
};

//------

class CQTagMarksTags : public QWidget {
  Q_OBJECT

 public:
  CQTagMarksTags(CQTagMarks *marks);
 ~CQTagMarksTags();

 private:
  void paintEvent(QPaintEvent *event);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

  QSize sizeHint() const { return QSize(500, 700); }

 private:
  CQTagMarks     *marks_        { nullptr };
  CQPixmapButton *left_pixmap_  { nullptr };
  CQPixmapButton *right_pixmap_ { nullptr };
  CQPixmapButton *up_pixmap_    { nullptr };
  CQPixmapButton *down_pixmap_  { nullptr };
};

//------

class CQTagMarksMarks : public QWidget {
  Q_OBJECT

 public:
  CQTagMarksMarks(CQTagMarks *marks);
 ~CQTagMarksMarks();

 private:
  void paintEvent(QPaintEvent *event);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent (QMouseEvent *event);

  void wheelEvent(QWheelEvent *event);

 private:
  CQTagMarks     *marks_       { nullptr };
  CQPixmapButton *up_pixmap_   { nullptr };
  CQPixmapButton *down_pixmap_ { nullptr };
};
