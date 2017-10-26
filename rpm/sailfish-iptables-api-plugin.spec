Name: sailfish-iptables-api-plugin
Version: 0.0.1
Release: 0
Summary: Sailfish Connman iptables api plugin
Group: Development/Libraries
License: GPLv2
URL: https://github.com/LaakkonenJussi/sailfish-connman-plugin-iptables-api
Source: %{name}-%{version}.tar.bz2
Requires: iptables
Requires: connman >= 1.31+git44
Requires: glib2
Requires: dbus >= 1.4
BuildRequires: iptables-devel
BuildRequires: connman-devel >= 1.31+git44
BuildRequires: pkgconfig(glib-2.0) >= 2.28
BuildRequires:  pkgconfig(dbus-1) >= 1.4
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
This package contains the Sailfish Connman iptables API plugin library.

%prep
%setup -q -n %{name}-%{version}

%build
make %{?jobs:-j%jobs} release

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/%{_libdir}/connman/plugins
mkdir -p %{buildroot}/usr/share/dbus-1/system.d/
install -m 644 src/sailfish-iptables-api.conf %{buildroot}/usr/share/dbus-1/system.d/
%preun

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/connman/plugins/sailfish-iptables-api-plugin.so
%config /usr/share/dbus-1/system.d/sailfish-iptables-api.conf

