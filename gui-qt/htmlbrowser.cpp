#include "htmlbrowser.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

void HtmlBrowser::viewSourceToggle()
{
    if (viewing_source) {
        // show rendered HTML
        setHtml(m_html_for_normal_and_source_view);
        viewing_source = false;
    } else {
        // show HTML source
        setPlainText(m_html_for_normal_and_source_view);
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

void HtmlBrowser::setHtmlForNormalAndSourceView(QString html_for_normal_and_source_view)
{
    m_html_for_normal_and_source_view = std::move(html_for_normal_and_source_view);
    setHtml(m_html_for_normal_and_source_view);
}
