#ifndef HTMLBROWSER_H
#define HTMLBROWSER_H

#include <QTextBrowser>

class HtmlBrowser : public QTextBrowser
{
    Q_OBJECT

    QAction * viewSourceAct;
    void viewSourceToggle();
    bool viewing_source = false;
    QString m_html_for_normal_and_source_view;

protected:
    void contextMenuEvent(QContextMenuEvent * event) override;

public:
    HtmlBrowser(QWidget * parent);

    // Set HTML which will not be modified in any way (used for source/rendered views).
    // The reason we need this is that toHtml() returns modified HTML code, not the same as was passed to setHtml() previously.
    void setHtmlForNormalAndSourceView(QString html_for_normal_and_source_view);
};

#endif // HTMLBROWSER_H
