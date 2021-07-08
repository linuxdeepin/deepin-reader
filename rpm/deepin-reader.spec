%global debug_package   %{nil}
%define specrelease 1%{?dist}
%if 0%{?openeuler}
%define specrelease 1
%endif

Name:           deepin-reader
Version:        5.9.0.26
Release:        %{specrelease}
Summary:        A simple PDF reader, supporting bookmarks, highlights and annotations
License:        GPLv3+
URL:            https://github.com/linuxdeepin/%{name}
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: qt5-devel

BuildRequires: pkgconfig(dtkwidget)
BuildRequires: pkgconfig(ddjvuapi)
BuildRequires: pkgconfig(nss)
BuildRequires: pkgconfig(libjpeg)
BuildRequires: pkgconfig(cairo)
BuildRequires: openjpeg2-devel
BuildRequires: poppler-qt5-devel
BuildRequires: libspectre-devel
BuildRequires: kf5-karchive-devel
BuildRequires: libtiff-devel
BuildRequires: freetype-devel
BuildRequires: nspr-devel
BuildRequires: fontconfig-devel
BuildRequires: lcms2-devel
BuildRequires: libX11-devel
# BuildRequires: libicu-devel
# BuildRequires: libpng-devel
# BuildRequires: zlib-devel
# BuildRequires: gtest-devel
# BuildRequires: libchardet-devel



%description
%{summary}.

%prep
%autosetup

%build
# help find (and prefer) qt5 utilities, e.g. qmake, lrelease
export PATH=%{_qt5_bindir}:$PATH
mkdir build && pushd build
%qmake_qt5 ../ DAPP_VERSION=%{version} DEFINES+="VERSION=%{version}"
%make_build
popd

%install
%make_install -C build INSTALL_ROOT="%buildroot"

%files
%doc README.md
%license LICENSE
%{_bindir}/%{name}
%{_datadir}/icons/hicolor/scalable/apps/%{name}.svg
%{_datadir}/%{name}/translations/*.qm
%{_datadir}/applications/%{name}.desktop
# /usr/lib/libdeepin-pdfium.*
%{_datadir}/deepin-manual/manual-assets/application/deepin-reader/document-viewer/*
/usr/lib/deepin-reader/libdeepin-poppler*

%changelog
* Thu May 6 2021 zhangdignwen <zhangdingwen@uniontech.com> - 5.9.0.26.10-1
- fix bug

* Thu Apr 16 2021 zhangdingwen <zhangdingwen@uniontech.com> - 5.9.0.26-1
- Initial release for OpenEuler

* Fri Sep 18 2020 guoqinglan <guoqinglan@uniontech.com> - 5.7.0.21-1
- add qmake_qt5 version
