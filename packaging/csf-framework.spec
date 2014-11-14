Summary:       Content Security Framework
Name:          csf-framework
Version:       2.0.2
Release:       0
License:       BSD-3-Clause
Group:         Security/Libraries
URL:           http://tizen.org
Source:        %{name}-%{version}.tar.gz
Source1001:    csf-framework.manifest
Source1002:    com.tsc.ipc.server.plugin.conf
Source1003:    com.tsc.ipc.server.wp.conf
Source1004:    tpcsserdaemon.service
Source1005:    twpserdaemon.service
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(libxml-2.0)

%description
A general purpose content screening and reputation solution.

%prep
%setup -q
cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .
cp %{SOURCE1004} .
cp %{SOURCE1005} .

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
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_unitdir}
mkdir -p %{buildroot}/%{_sysconfdir}/dbus-1/system.d
install -D framework/lib/libsecfw.so %{buildroot}%{_libdir}/
install -D framework/lib/libscclient.so %{buildroot}%{_libdir}/
install -D framework/lib/libscserver.so %{buildroot}%{_libdir}/
install -D framework/bin/TPCSSerDaemon %{buildroot}%{_bindir}/
install -D framework/bin/TWPSerDaemon %{buildroot}%{_bindir}/
install -m0644 %{SOURCE1002} %{buildroot}%{_sysconfdir}/dbus-1/system.d/
install -m0644 %{SOURCE1003} %{buildroot}%{_sysconfdir}/dbus-1/system.d/
install -m0644 %{SOURCE1004} %{buildroot}%{_unitdir}
install -m0644 %{SOURCE1005} %{buildroot}%{_unitdir}

%post
/sbin/ldconfig
systemctl daemon-reload
if [ $1 = 1 ]; then
    systemctl restart tpcsserdaemon.service
    systemctl restart twpserdaemon.service
    systemctl enable tpcsserdaemon.service -q
    systemctl enable twpserdaemon.service -q
fi

%preun
if [ $1 = 0 ]; then
    systemctl stop tpcsserdaemon.service
    systemctl stop twpserdaemon.service
    systemctl disable tpcsserdaemon.service -q
    systemctl disable twpserdaemon.service -q
fi

%postun
/sbin/ldconfig
if [ $1 = 0 ]; then
    systemctl daemon-reload
fi
rm -fr /usr/bin/tpcs_config.dtd
rm -fr /usr/bin/tpcs_config.xml

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libsecfw.so
%{_libdir}/libscclient.so
%{_libdir}/libscserver.so
%defattr(0755,root,root)
%{_bindir}/TPCSSerDaemon
%{_bindir}/TWPSerDaemon
%defattr(0644,root,root)
%config %{_sysconfdir}/dbus-1/system.d/com.tsc.ipc.server.plugin.conf
%config %{_sysconfdir}/dbus-1/system.d/com.tsc.ipc.server.wp.conf
%{_unitdir}/tpcsserdaemon.service
%{_unitdir}/twpserdaemon.service
