Summary:       Content Security Framework
Name:          csf-framework
Version:       2.0.2.1
Release:       0
License:       BSD-3-Clause
Group:         Security/Libraries
URL:           http://tizen.org
Source:        %{name}-%{version}.tar.gz
Source1001:    csf-framework.manifest
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(dlog)

%description
A general purpose content screening and reputation solution.

%package devel
Summary:  Development files
Group:    Development/Libraries
Requires: %name = %version-%release

%description devel
Development files

%prep
%setup -q
cp %{SOURCE1001} .

%build
make -C framework -f Makefile all VERSION=%version PREFIX=%_prefix

%install
rm -rf %{buildroot}
mkdir -p %buildroot%_includedir/csf
mkdir -p %buildroot%_libdir/pkgconfig

make install -C framework \
             LIB_DESTDIR=%buildroot%_libdir \
             HEADER_DESTDIR=%buildroot%_includedir/csf \
             VERSION=%version

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE
%{_libdir}/libsecfw.so.*

%files devel
%_includedir/csf/TCSErrorCodes.h
%_includedir/csf/TCSImpl.h
%_includedir/csf/TWPImpl.h
%_libdir/pkgconfig/%name.pc
%_libdir/libsecfw.so
