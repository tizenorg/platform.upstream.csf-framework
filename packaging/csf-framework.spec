Summary:       Content Security Framework
Name:          csf-framework
Version:       2.0.2
Release:       0
License:       BSD-3-Clause
Group:         Security/Libraries
URL:           http://tizen.org
Source:        %{name}-%{version}.tar.gz
Source1001:    csf-framework.manifest
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(libxml-2.0)

%description
A general purpose content screening and reputation solution.

%prep
%setup -q
cp %{SOURCE1001} .

%build
cd framework

# Build Framework Library
make -f Makefile all

# Build IPC Client Library
make -f Makefile_channel_client all

# Build IPC Server Library
make -f Makefile_channel_server all

# Build Plugin Control Service
make -f Makefile_TPCSSerDaemon all

# Build Web Protection Control Service
make -f Makefile_TWPSerDaemon all

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_libdir}/
mkdir -p %{buildroot}%{_bindir}/
install -D framework/lib/libsecfw.so %{buildroot}%{_libdir}/
install -D framework/lib/libscclient.so %{buildroot}%{_libdir}/
install -D framework/lib/libscserver.so %{buildroot}%{_libdir}/
install -D framework/bin/TPCSSerDaemon %{buildroot}%{_bindir}/
install -D framework/bin/TWPSerDaemon %{buildroot}%{_bindir}/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libsecfw.so
%{_libdir}/libscclient.so
%{_libdir}/libscserver.so
%attr(755,root,root) %{_bindir}/TPCSSerDaemon
%attr(755,root,root) %{_bindir}/TWPSerDaemon

