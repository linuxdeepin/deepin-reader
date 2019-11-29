#include "FileAttrWidget.h"

#include "frame/DocummentFileHelper.h"
#include "AttrScrollWidget.h"
#include <DWindowCloseButton>
#include <QFileInfo>
#include "controller/DataManager.h"
#include <DFontSizeManager>
#include <QMdiArea>
#include <DFrame>
#include <QTextLayout>
#include <QTextOption>
//#include <private/qtextengine_p.h>

FileAttrWidget::FileAttrWidget(DWidget *parent)
    : DAbstractDialog(parent)
{
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置
    setFixedSize(QSize(400, 642));

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

//  各个 对应的 label 赋值
void FileAttrWidget::setFileAttr()
{
    auto dproxy = DocummentFileHelper::instance();
    if (nullptr == dproxy) {
        return;
    }

    QImage image;
    bool rl = dproxy->getImage(0, image, 94, 113);
    if (rl) {
        labelImage->setPixmap(QPixmap::fromImage(image));
    }

    QString filePath = DataManager::instance()->strOnlyFilePath();
    QFileInfo info(filePath);
    QString szTitle = info.fileName();
    m_strFileName = szTitle;

    labelFileName->setText(szTitle);//elideText(szTitle, labelFileName->size(), QTextOption::WrapAtWordBoundaryOrAnywhere, labelFileName->font(), Qt::ElideMiddle, 0));

    auto hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(10, 35, 10, 10);

    auto scroll = new DScrollArea(this);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidget(new AttrScrollWidget);
    scroll->setWidgetResizable(true);

    hLayout->addWidget(scroll);

    m_pVBoxLayout->addItem(hLayout);
}

void FileAttrWidget::showScreenCenter()
{
    setFileAttr();

    Dtk::Widget::moveToCenter(this);
    this->show();
}

void FileAttrWidget::initWidget()
{
    initCloseBtn();

    initImageLabel();
}

void FileAttrWidget::initCloseBtn()
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);

    auto closeButton = new DWindowCloseButton(this);
    closeButton->setFixedSize(QSize(50, 50));
    closeButton->setIconSize(QSize(50, 50));
    closeButton->setToolTip(tr("close"));
    connect(closeButton, &DWindowCloseButton::clicked, this, &FileAttrWidget::slotBtnCloseClicked);

    layout->addWidget(closeButton);

    m_pVBoxLayout->addItem(layout);
}

void FileAttrWidget::initImageLabel()
{
    labelImage = new DLabel(this);
    labelImage->setAlignment(Qt::AlignCenter);

    labelFileName = new DLabel("", this)/*DTextEdit*/;
    DFontSizeManager::instance()->bind(labelFileName, DFontSizeManager::T8);
    labelFileName->setAlignment(Qt::AlignCenter);
    labelFileName->setWordWrap(true);
    labelFileName->setMaximumWidth(400);
    labelFileName->setMinimumHeight(54);

    auto vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 6, 0, 30);
    vlayout->setSpacing(10);
    vlayout->addWidget(labelImage);
    vlayout->addWidget(labelFileName);

    m_pVBoxLayout->addItem(vlayout);
}

#if 0
QString FileAttrWidget::elideText(const QString &text, const QSizeF &size, QTextOption::WrapMode wordWrap, const QFont &font, Qt::TextElideMode mode, qreal lineHeight, qreal flags)
{
    QTextLayout textLayout(text);

    textLayout.setFont(font);

    QStringList lines;

    elideText(&textLayout, size, wordWrap, mode, lineHeight, 0, &lines);

    return lines.join('\n');
}

void FileAttrWidget::elideText(QTextLayout *layout, const QSizeF &size, QTextOption::WrapMode wordWrap
                               , Qt::TextElideMode mode, qreal lineHeight, int flags
                               , QStringList *lines, QPainter *painter, QPointF offset
                               , const QColor &shadowColor, const QPointF &shadowOffset
                               , const QBrush &background, /*qreal backgroundReaius,*/ QList<QRectF> *boundingRegion)
{
    qreal height = 0;
    bool drawBackground = background.style() != Qt::NoBrush;
    bool drawShadow = shadowColor.isValid();

    QString text = layout->engine()->hasFormats() ? layout->engine()->block.text() : layout->text();
    QTextOption &text_option = *const_cast<QTextOption*>(&layout->textOption());

    text_option.setWrapMode(wordWrap);

    if (flags & Qt::AlignRight)
        text_option.setAlignment(Qt::AlignRight);
    else if (flags & Qt::AlignHCenter)
        text_option.setAlignment(Qt::AlignHCenter);

    if (painter) {
        text_option.setTextDirection(painter->layoutDirection());
        layout->setFont(painter->font());
    } else {
        // dont paint
        layout->engine()->ignoreBidi = true;
    }

    auto naturalTextRect = [&] (const QRectF rect) {
        QRectF new_rect = rect;

        new_rect.setHeight(lineHeight);

        return new_rect;
    };

    auto drawShadowFun = [&] (const QTextLine &line) {
        const QPen pen = painter->pen();

        painter->setPen(shadowColor);
        line.draw(painter, shadowOffset);

        // restore
        painter->setPen(pen);
    };

    layout->beginLayout();

    QTextLine line = layout->createLine();
    QRectF lastLineRect;

    while (line.isValid()) {
        height += lineHeight;

        if (height + lineHeight > size.height()) {
            const QString &end_str = layout->engine()->elidedText(mode, qRound(size.width()), flags, line.textStart());

            layout->endLayout();
            layout->setText(end_str);

            if (layout->engine()->block.docHandle()) {
                const_cast<QTextDocument*>(layout->engine()->block.document())->setPlainText(end_str);
            }

            text_option.setWrapMode(QTextOption::NoWrap);
            layout->beginLayout();
            line = layout->createLine();
            line.setLineWidth(size.width() - 1);
            text = end_str;
        } else {
            line.setLineWidth(size.width());
        }

        line.setPosition(offset);

        const QRectF rect = naturalTextRect(line.naturalTextRect());

        if (painter) {
            if (drawBackground) {
                const QMarginsF margins(4/*backgroundRadius*/, 0, 4/*backgroundRadius*/, 0);
                QRectF backBounding = rect;
                QPainterPath path;

                if (lastLineRect.isValid()) {
                    if (qAbs(rect.width() - lastLineRect.width()) < 4/*backgroundRadius*/ * 2) {
                        backBounding.setWidth(lastLineRect.width());
                        backBounding.moveCenter(rect.center());
                        path.moveTo(lastLineRect.x() - 4/*backgroundRadius*/, lastLineRect.bottom() - 4/*backgroundRadius*/);
                        path.lineTo(lastLineRect.x(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right() + 4/*backgroundRadius*/, lastLineRect.bottom() - 4/*backgroundRadius*/);
                        path.lineTo(lastLineRect.right() + 4/*backgroundRadius*/, backBounding.bottom() - 4/*backgroundRadius*/);
                        path.arcTo(backBounding.right() - 4/*backgroundRadius*/, backBounding.bottom() - 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 0, -90);
                        path.lineTo(backBounding.x(), backBounding.bottom());
                        path.arcTo(backBounding.x() - 4/*backgroundRadius*/, backBounding.bottom() - 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 270, -90);
                        lastLineRect = backBounding;
                    } else if (lastLineRect.width() > rect.width()) {
                        backBounding += margins;
                        path.moveTo(backBounding.x() - 4/*backgroundRadius*/, backBounding.y() - 1);
                        path.arcTo(backBounding.x() - 4/*backgroundRadius*/ * 2, backBounding.y() - 1, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2 + 1, 90, -90);
                        path.lineTo(backBounding.x(), backBounding.bottom() - 4/*backgroundRadius*/);
                        path.arcTo(backBounding.x(), backBounding.bottom() - 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 180, 90);
                        path.lineTo(backBounding.right() - 4/*backgroundRadius*/, backBounding.bottom());
                        path.arcTo(backBounding.right() - 4/*backgroundRadius*/ * 2, backBounding.bottom() - 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2, 270, 90);
                        path.lineTo(backBounding.right(), backBounding.top() + 4/*backgroundRadius*/);
                        path.arcTo(backBounding.right(), backBounding.top() - 1, 4/*backgroundRadius*/ * 2, 4/*backgroundRadius*/ * 2 + 1, 180, -90);
                        path.closeSubpath();
                        lastLineRect = rect;
                    } else {
                        backBounding += margins;
                        path.moveTo(lastLineRect.x() - 4 * 2, lastLineRect.bottom());
                        path.arcTo(lastLineRect.x() - 4 * 3, lastLineRect.bottom() - 4 * 2, 4 * 2, 4 * 2, 270, 90);
                        path.lineTo(lastLineRect.x(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right(), lastLineRect.bottom() - 1);
                        path.lineTo(lastLineRect.right() + 4, lastLineRect.bottom() - 4 * 2);
                        path.arcTo(lastLineRect.right() + 4, lastLineRect.bottom() - 4 * 2, 4 * 2, 4 * 2, 180, 90);

                        path.addRoundedRect(backBounding, 4, 4);
                        lastLineRect = rect;
                    }
                } else {
                    lastLineRect = backBounding;
                    path.addRoundedRect(backBounding + margins, 4, 4);
                }

                bool a = painter->testRenderHint(QPainter::Antialiasing);
                qreal o = painter->opacity();

                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->setOpacity(1);
                painter->fillPath(path, background);
                painter->setRenderHint(QPainter::Antialiasing, a);
                painter->setOpacity(o);
            }

            if (drawShadow) {
                drawShadowFun(line);
            }

            line.draw(painter, QPointF(0, 0));
        }

        if (boundingRegion) {
            boundingRegion->append(rect);
        }

        offset.setY(offset.y() + lineHeight);

        if (lines) {
            lines->append(text.mid(line.textStart(), line.textLength()));
        }

        if (height + lineHeight > size.height())
            break;

        line = layout->createLine();
    }

    layout->endLayout();
}
#endif

QString FileAttrWidget::calcText(const QFont &font, const QString &filename, const QSize &size)
{
#if 1
    QString text;
    QString tempText;
    int totalHeight = size.height();
    int lineWidth = size.width() - 12;

    QFontMetrics fm(font);

    int calcHeight = 0;
    int lineHeight = fm.height();
    int lineSpace = 0;
    int lineCount = (totalHeight - lineSpace) / lineHeight;
    int prevLineCharIndex = 0;
    for (int charIndex = 0; charIndex < filename.size() && lineCount >= 0; ++charIndex) {
        int fmWidth = fm.horizontalAdvance(tempText);
        if (fmWidth > lineWidth) {
            calcHeight += lineHeight/*+3*/;
            if (calcHeight + lineHeight > totalHeight) {
                QString endString = filename.mid(prevLineCharIndex);
                const QString &endText = fm.elidedText(endString, Qt::ElideRight, size.width());
                text += endText;

                lineCount = 0;
                break;
            }

            QChar currChar = tempText.at(tempText.length() - 1);
            QChar nextChar = filename.at(filename.indexOf(tempText) + tempText.length());
            if (currChar.isLetter() && nextChar.isLetter()) {
//                tempText += '-';
            }
            fmWidth = fm.horizontalAdvance(tempText);
            if (fmWidth > size.width()) {
                --charIndex;
                --prevLineCharIndex;
                tempText = tempText.remove(tempText.length() - 2, 1);
            }
            text += tempText;

            --lineCount;
            if (lineCount > 0) {
                text += "\n";
            }
            tempText = filename.at(charIndex);

            prevLineCharIndex = charIndex;
        } else {
            tempText += filename.at(charIndex);
        }
    }

    if (lineCount > 0) {
        text += tempText;
    }

    return text;
#endif
}

void FileAttrWidget::slotBtnCloseClicked()
{
    this->close();
}

void FileAttrWidget::paintEvent(QPaintEvent *e)
{
    DAbstractDialog::paintEvent(e);

    if(labelFileName){
        QString fileName{};
        QString fileSuffix{};
        QString text{};
        QString fileFullName = m_strFileName;
        QFileInfo info(fileFullName);
        fileName = info.fileName();
        fileSuffix = info.suffix();
        text = calcText(labelFileName->font(), fileName, labelFileName->size());
        text += fileSuffix;
        labelFileName->setText(text);
    }
}
