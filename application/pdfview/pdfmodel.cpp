
#include "pdfmodel.h"
#include <poppler-qt5.h>
#include <poppler-form.h>
#include <QSettings>

namespace Defaults
{
const bool antialiasing = true;

const bool textAntialiasing = true;

const int textHinting = 0;

const bool ignorePaperColor = false;

const bool overprintPreview = false;

const int thinLineMode = 0;

const int backend = 0;

} // Defaults


 PdfAnnotation::PdfAnnotation(QMutex *mutex, Poppler::Annotation *annotation):
    m_mutex(mutex),
    m_annotation(annotation)
{

}

 PdfAnnotation::~PdfAnnotation()
{
 delete m_annotation;
}

QRectF  PdfAnnotation::boundary() const
{
    LOCK_ANNOTATION
    return m_annotation->boundary().normalized();
}

QString  PdfAnnotation::contents() const
{
    LOCK_ANNOTATION
    return m_annotation->contents();
}

QWidget * PdfAnnotation::createWidget()
{
    QWidget* widget = 0;

//    if(m_annotation->subType() == Poppler::Annotation::AText || m_annotation->subType() == Poppler::Annotation::AHighlight)
//    {
//        widget = new AnnotationWidget(m_mutex, m_annotation);

//        connect(widget, SIGNAL(wasModified()), SIGNAL(wasModified()));
//    }
//    else if(m_annotation->subType() == Poppler::Annotation::AFileAttachment)
//    {
//        widget = new FileAttachmentAnnotationWidget(m_mutex, static_cast< Poppler::FileAttachmentAnnotation* >(m_annotation));
//    }

//    connect(this, SIGNAL(destroyed()), widget, SLOT(deleteLater()));

    return widget;
}


 PdfFormField::PdfFormField(QMutex *mutex, Poppler::FormField *formField):
    m_mutex(mutex),
    m_formField(formField)
{

}

 PdfFormField::~PdfFormField()
{
    delete m_formField;
}

QRectF  PdfFormField::boundary() const
{
    LOCK_FORM_FIELD
    return m_formField->rect().normalized();
}

QString  PdfFormField::name() const
{
    LOCK_FORM_FIELD
    return m_formField->name();
}

QWidget * PdfFormField::createWidget()
{
    QWidget* widget = 0;

//    if(m_formField->type() == Poppler::FormField::FormText)
//    {
//        Poppler::FormFieldText* formFieldText = static_cast< Poppler::FormFieldText* >(m_formField);

//        if(formFieldText->textType() == Poppler::FormFieldText::Normal)
//        {
//            widget = new NormalTextFieldWidget(m_mutex, formFieldText);
//        }
//        else if(formFieldText->textType() == Poppler::FormFieldText::Multiline)
//        {
//            widget = new MultilineTextFieldWidget(m_mutex, formFieldText);
//        }
//    }
//    else if(m_formField->type() == Poppler::FormField::FormChoice)
//    {
//        Poppler::FormFieldChoice* formFieldChoice = static_cast< Poppler::FormFieldChoice* >(m_formField);

//        if(formFieldChoice->choiceType() == Poppler::FormFieldChoice::ComboBox)
//        {
//            widget = new ComboBoxChoiceFieldWidget(m_mutex, formFieldChoice);
//        }
//        else if(formFieldChoice->choiceType() == Poppler::FormFieldChoice::ListBox)
//        {
//            widget = new ListBoxChoiceFieldWidget(m_mutex, formFieldChoice);
//        }
//    }
//    else if(m_formField->type() == Poppler::FormField::FormButton)
//    {
//        Poppler::FormFieldButton* formFieldButton = static_cast< Poppler::FormFieldButton* >(m_formField);

//        if(formFieldButton->buttonType() == Poppler::FormFieldButton::CheckBox)
//        {
//            widget = new CheckBoxChoiceFieldWidget(m_mutex, formFieldButton);
//        }
//        else if(formFieldButton->buttonType() == Poppler::FormFieldButton::Radio)
//        {
//            widget = new RadioChoiceFieldWidget(m_mutex, formFieldButton);
//        }
//    }

//    connect(widget, SIGNAL(wasModified()), SIGNAL(wasModified()));

    return widget;
}


 PdfPage::PdfPage(QMutex *mutex, Poppler::Page *page):
    m_mutex(mutex),
    m_page(page)
{

}
 PdfPage::~PdfPage()
{
//    textCache()->remove(this);
     //    delete m_page;
 }

 QSizeF PdfPage::size() const
 {
     LOCK_PAGE
     return m_page->pageSizeF();
 }

 QImage PdfPage::render(qreal horizontalResolution, qreal verticalResolution, Rotation rotation, QRect boundingRect) const
 {
     LOCK_PAGE

     Poppler::Page::Rotation rotate;

     switch(rotation)
     {
     default:
     case RotateBy0:
         rotate = Poppler::Page::Rotate0;
         break;
     case RotateBy90:
         rotate = Poppler::Page::Rotate90;
         break;
     case RotateBy180:
         rotate = Poppler::Page::Rotate180;
         break;
     case RotateBy270:
         rotate = Poppler::Page::Rotate270;
         break;
     }

     int x = -1;
     int y = -1;
     int w = -1;
     int h = -1;

     if(!boundingRect.isNull())
     {
         x = boundingRect.x();
         y = boundingRect.y();
         w = boundingRect.width();
         h = boundingRect.height();
     }

     return m_page->renderToImage(horizontalResolution, verticalResolution, x, y, w, h, rotate);
      //return m_page->renderToImage(90, 90, x, y, w, h, rotate);
 }

 PdfDocument::~PdfDocument()
 {

 }

 int PdfDocument::numberOfPages() const
 {
     LOCK_DOCUMENT
      return m_document->numPages();
 }

 ModelPage *PdfDocument::page(int index) const
 {
     LOCK_DOCUMENT

     if(Poppler::Page* page = m_document->page(index))
     {
         return new PdfPage(&m_mutex, page);
     }

     return 0;
 }

 bool PdfDocument::isLocked() const
 {
     LOCK_DOCUMENT

     return m_document->isLocked();
 }

 PdfDocument::PdfDocument(Poppler::Document *document):
     m_mutex(),
     m_document(document)
 {

 }

 PdfPlugin::PdfPlugin(QObject *parent): QObject(parent)
 {
      setObjectName("PdfPlugin");
      m_settings = new QSettings("qpdfview", "pdf-plugin", this);
 }

 ModelDocument *PdfPlugin::loadDocument(const QString &filePath) const
 {
     if(Poppler::Document* document = Poppler::Document::load(filePath))
     {
         int pages=0;
         document->setRenderHint(Poppler::Document::Antialiasing, m_settings->value("antialiasing", Defaults::antialiasing).toBool());
         document->setRenderHint(Poppler::Document::TextAntialiasing, m_settings->value("textAntialiasing", Defaults::textAntialiasing).toBool());

         switch(m_settings->value("textHinting", Defaults::textHinting).toInt())
         {
         default:
         case 0:
             document->setRenderHint(Poppler::Document::TextHinting, false);
             break;
         case 1:
             document->setRenderHint(Poppler::Document::TextHinting, true);
             document->setRenderHint(Poppler::Document::TextSlightHinting, false);
             break;
         case 2:
             document->setRenderHint(Poppler::Document::TextHinting, true);
             document->setRenderHint(Poppler::Document::TextSlightHinting, true);
             break;
         }

         document->setRenderHint(Poppler::Document::IgnorePaperColor, m_settings->value("ignorePaperColor", Defaults::ignorePaperColor).toBool());
         document->setRenderHint(Poppler::Document::OverprintPreview, m_settings->value("overprintPreview", Defaults::overprintPreview).toBool());
         switch(m_settings->value("thinLineMode", Defaults::thinLineMode).toInt())
         {
         default:
         case 0:
             document->setRenderHint(Poppler::Document::ThinLineSolid, false);
             document->setRenderHint(Poppler::Document::ThinLineShape, false);
             break;
         case 1:
             document->setRenderHint(Poppler::Document::ThinLineSolid, true);
             document->setRenderHint(Poppler::Document::ThinLineShape, false);
             break;
         case 2:
             document->setRenderHint(Poppler::Document::ThinLineSolid, false);
             document->setRenderHint(Poppler::Document::ThinLineShape, true);
             break;
         }
         switch(m_settings->value("backend", Defaults::backend).toInt())
         {
         default:
         case 0:
             document->setRenderBackend(Poppler::Document::SplashBackend);
             break;
         case 1:
             document->setRenderBackend(Poppler::Document::ArthurBackend);
             break;
         }

        return new PdfDocument(document);
     }
     return 0;
 }



