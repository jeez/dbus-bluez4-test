Name:       dbus-bluez4-test
Version:    0.1
Release:    0
License:    BSD-3-Clause
Group:      Development/Libraries
Summary:    Tizen Test for DBUS System BUS
Source0:    %{name}-%{version}.tar.gz
Source1001: %{name}.manifest

BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(glib-2.0)
Requires:      crosswalk

%description
Testing the access to the DBUS System Bus

%prep
%setup -q
cp %{SOURCE1001} .

%build
g++ -o %{name} `pkg-config --libs --cflags gio-2.0` %{name}.cc

%install
install -D -p -m 755 %{name} %{buildroot}%{_bindir}/%{name}

%files
%{_bindir}/%{name}

%post
/usr/bin/chsmack -a "_" -e "dbus-bluez4-test" %{_bindir}/%{name}
