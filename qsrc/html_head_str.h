const char *html_head_str = "\
<html>\n\
\n\
<head>\n\
\n\
<title>Bookmarks</title>\n\
\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n\
\n\
<!-- ============== Style Sheet ============== -->\n\
\n\
<style type=\"text/css\">\n\
\n\
/*** The Essential Code ***/\n\
\n\
body {\n\
  min-width: 550px;         /* 2 x LC width + RC width */\n\
}\n\
\n\
#container {\n\
  padding-left: 120px;      /* LC width */\n\
  padding-right: 120px;     /* RC width */\n\
}\n\
\n\
#container .column {\n\
  font-size: small;\n\
  position: relative;\n\
  float: left;\n\
}\n\
\n\
#center {\n\
  width: 100%;\n\
}\n\
\n\
#left {\n\
  width: 120px;             /* LC width */\n\
  right: 120px;             /* LC width */\n\
  margin-left: -100%;\n\
}\n\
\n\
#right {\n\
  width: 120px;             /* RC width */\n\
  margin-right: -100%;\n\
}\n\
\n\
/*** IE6 Fix ***/\n\
* html #left {\n\
  left: 120px;              /* RC width */\n\
}\n\
\n\
/*** Just for Looks ***/\n\
\n\
body {\n\
  margin: 0;\n\
  padding: 0 0;\n\
  background: #FFFFFF;\n\
}\n\
\n\
#left {\n\
  padding: 0 0;\n\
  background: #69C;\n\
}\n\
\n\
#center {\n\
  padding: 0 0;\n\
  background: #C0C0C0;\n\
}\n\
\n\
#right {\n\
  padding: 0 0;\n\
  background: #C0C0C0;\n\
}\n\
\n\
#container {\n\
  overflow: hidden;\n\
}\n\
\n\
#container .column {\n\
  padding-bottom: 5010px; /* X + padding-bottom */\n\
  margin-bottom: -5000px; /* X */\n\
}\n\
\n\
h1 {\n\
  font-size: 2.0em;\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
  font-weight: normal;\n\
  color: #59708C;\n\
  border-bottom: 1px solid #59708C;\n\
}\n\
\n\
h2 {\n\
  font-size: 1.5em;\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
  font-weight: normal;\n\
  color: #59708C;\n\
}\n\
\n\
h3 {\n\
  font-size: 1.0em;\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
  font-weight: normal;\n\
  color: #59708C;\n\
}\n\
\n\
p {\n\
  font-size: 0.9em;\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
  font-weight: normal;\n\
}\n\
\n\
a {\n\
  font-size: 0.9em;\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
}\n\
\n\
dl {\n\
  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\
}\n\
\n\
#center dl {\n\
  padding: 0 10px 10px 10px;\n\
}\n\
\n\
#left dl {\n\
  padding: 0 0 0 0;\n\
}\n\
\n\
</style>\n\
\n\
</head>\n\
\n\
<body>\n\
";
