%global upstreamname rocSPARSE

%global toolchain rocm
# hipcc does not support some clang flags
%global build_cxxflags %(echo %{optflags} | sed -e 's/-fstack-protector-strong/-Xarch_host -fstack-protector-strong/' -e 's/-fcf-protection/-Xarch_host -fcf-protection/')

# $gpu will be evaluated in the loops below             
%global _vpath_builddir %{_vendor}-%{_target_os}-build-${gpu}

# Tests are downloaded so this option is only good for local building
# Also need to
# export QA_RPATHS=0xff
%bcond_with test

Name:           rocsparse
Version:        %{rocm_version}
Release:        1%{?dist}
Summary:        SPARSE implementation for ROCm
Url:            https://github.com/ROCmSoftwarePlatform
License:        MIT

Source0:        %{url}/%{upstreamname}/archive/refs/tags/rocm-%{rocm_version}.tar.gz#/%{upstreamname}-%{rocm_version}.tar.gz
Patch0:         0001-prepare-rocsparse-cmake-for-fedora.patch

BuildRequires:  rocm-rpm-macros
BuildRequires:  %rocm_buildrequires
BuildRequires:  rocblas-devel
BuildRequires:  rocprim-devel

%if %{with test}
BuildRequires:  %rocm_buildrequires_test
BuildRequires:  libomp-devel
%endif

Requires:       %rocm_requires

%description
rocSPARSE exposes a common interface that provides Basic
Linear Algebra Subroutines for sparse computation
implemented on top of AMD's Radeon Open eCosystem Platform
ROCm runtime and toolchains. rocSPARSE is created using
the HIP programming language and optimized for AMD's
latest discrete GPUs.

%package devel
Summary: Libraries and headers for %{name}

%description devel
%{summary}

%if %{with test}
%package test
Summary:        Tests for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description test
%{summary}
%endif

%prep
%autosetup -p1 -n %{upstreamname}-rocm-%{version}

%build
for gpu in %{rocm_gpu_list}
do
    module load rocm/$gpu
    %cmake %rocm_cmake_options \
%if %{with test}
           %rocm_cmake_test_options \
%endif
           -DBUILD_WITH_TENSILE=OFF

    %cmake_build
    module purge
done

%install
for gpu in %{rocm_gpu_list}
do
    %cmake_install
done

%files
%dir %{_libdir}/rocm/
%license LICENSE.md
%exclude %{_docdir}/%{name}/LICENSE.md
%{_libdir}/lib%{name}.so
%{_libdir}/rocm/gfx8/lib/lib%{name}.so
%{_libdir}/rocm/gfx9/lib/lib%{name}.so
%{_libdir}/rocm/gfx10/lib/lib%{name}.so
%{_libdir}/rocm/gfx11/lib/lib%{name}.so

%files devel
%doc README.md
%{_includedir}/%{name}
%{_libdir}/cmake/%{name}/
%{_libdir}/lib%{name}.so.*
%{_libdir}/rocm/gfx8/lib/lib%{name}.so.*
%{_libdir}/rocm/gfx8/lib/cmake/%{name}/
%{_libdir}/rocm/gfx9/lib/lib%{name}.so.*
%{_libdir}/rocm/gfx9/lib/cmake/%{name}/
%{_libdir}/rocm/gfx10/lib/lib%{name}.so.*
%{_libdir}/rocm/gfx10/lib/cmake/%{name}/
%{_libdir}/rocm/gfx11/lib/lib%{name}.so.*
%{_libdir}/rocm/gfx11/lib/cmake/%{name}/

%if %{with test}
%files test
%{_bindir}/%{name}*
%{_libdir}/rocm/gfx8/bin/%{name}*
%{_libdir}/rocm/gfx9/bin/%{name}*
%{_libdir}/rocm/gfx10/bin/%{name}*
%{_libdir}/rocm/gfx11/bin/%{name}*
%endif

%changelog
* Sun Oct 8 2023 Tom Rix <trix@redhat.com>  - 5.7.0-1
- Initial package
