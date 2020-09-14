#include "htmlbrowser.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

void HtmlBrowser::viewSourceToggle()
{
    if (viewing_source) {
        // show rendered HTML
        setHtml(unchangable_html);
        viewing_source = false;
    } else {
        // show HTML source
        setPlainText(unchangable_html);
        viewing_source = true;
    }
}

void HtmlBrowser::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu menu(this);
    menu.addAction(viewSourceAct);
    menu.exec(event->globalPos());
}

HtmlBrowser::HtmlBrowser(QWidget * parent) : QTextBrowser(parent)
{
    viewSourceAct = new QAction(tr("View/hide HTML so&urce"), this);
    viewSourceAct->setShortcut(tr("Ctrl+U"));
    viewSourceAct->setCheckable(true);
    parent->addAction(viewSourceAct);
    connect(viewSourceAct, &QAction::triggered, this, &HtmlBrowser::viewSourceToggle);
}

void HtmlBrowser::setUnchangableHtml(QString html)
{
    unchangable_html = html;
    setHtml(html);
}
