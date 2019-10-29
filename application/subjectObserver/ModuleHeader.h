#ifndef MODULEHEADER_H
#define MODULEHEADER_H

#include <QString>

namespace ImageModule {

const QString g_normal_state = "normal";
const QString g_hover_state = "hover";
const QString g_press_state = "press";
const QString g_checked_state = "checked";
const QString g_focus_state = "focus";
const QString g_disable_state = "disable";

const QString g_dark_theme = "dark";
const QString g_light_theme = "light";
const QString g_system_theme = "system";

}

namespace  Constant {
const QString sAcknowledgementLink = "https://www.deepin.org/acknowledgments/deepin_reader";

const QString sPdf_Filter = "Pdf File (*.pdf)";
const QString sTiff_Filter = "Tiff files (*.tiff)";
const QString sPs_Filter = "Ps files (*.ps)";
const QString sXps_Filter = "Xps files (*.xps)";
const QString sDjvu_Filter = "Djvu files (*.djvu)";
const QString sQStringSep = "@#&!@#$";

const QString sLogoPath = ":/resources/logo/deepin-reader.svg";
}

namespace Pri {
const QString g_controller = "controller";
const QString g_docview = "docview";
const QString g_frame = "frame";
const QString g_mainShow = "mainShow";
const QString g_pdfControl = "pdfControl";
const QString g_subjectObserver = "subjectObserver";
}

#endif // IMAGEHEADER_H
