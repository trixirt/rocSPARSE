%global upstreamname rocSPARSE
%global rocm_release 5.6
%global rocm_patch 0
%global rocm_version %{rocm_release}.%{rocm_patch}

# clang and gfortran do not play well together
%global build_cxxflags %{nil}
%global build_ldflags %{nil}
%global debug_package %{nil}

# Create a *-test rpm to do testing with
# Limited to gfx11
#
# Because the binaries are linking against a nonstandard libpath
# it is necessary to
# export QA_RPATHS=0xff
#
# Tests are downloaded so this option is only good for local building
%bcond_with test

Name:           rocsparse
Version:        %{rocm_version}
Release:        1%{?dist}
Summary:        SPARSE implementation for ROCm
Url:            https://github.com/ROCmSoftwarePlatform
License:        MIT

Source0:        %{url}/%{upstreamname}/archive/refs/tags/rocm-%{version}.tar.gz#/%{upstreamname}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  clang-devel
BuildRequires:  compiler-rt
BuildRequires:  gcc-gfortran
BuildRequires:  lld
BuildRequires:  llvm-devel
BuildRequires:  rocblas-devel
BuildRequires:  rocm-cmake
BuildRequires:  rocm-comgr-devel
BuildRequires:  rocm-hip-devel
BuildRequires:  rocm-runtime-devel
BuildRequires:  rocprim-devel

%if %{with test}
BuildRequires:  gtest-devel
BuildRequires:  libomp-devel
%endif

%description
rocSPARSE exposes a common interface that provides Basic
Linear Algebra Subroutines for sparse computation
implemented on top of AMD's Radeon Open eCosystem Platform
ROCm runtime and toolchains. rocSPARSE is created using
the HIP programming language and optimized for AMD's
latest discrete GPUs.

%package devel
Summary: Libraries and headers for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

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
%cmake \
       -DBUILD_FILE_REORG_BACKWARD_COMPATIBILITY=OFF \
%if %{with test}
        -DAMDGPU_TARGETS=gfx1100,gfx1101,gfx1102 \
        -DBUILD_CLIENTS_BENCHMARKS=ON \
        -DBUILD_CLIENTS_TESTS=ON \
%endif
       -DCMAKE_CXX_COMPILER=hipcc \
       -DCMAKE_INSTALL_LIBDIR=%{_lib} \
       -DROCM_SYMLINK_LIBS=OFF
      
%cmake_build

%install
%cmake_install

%files
%license LICENSE.md
%exclude %{_docdir}/%{name}/LICENSE.md
%{_libdir}/lib%{name}.so.*

%files devel
%doc README.md
%{_includedir}/%{name}
%{_libdir}/lib%{name}.so
%{_libdir}/cmake/%{name}

%if %{with test}
%files test
%{_bindir}/rocsparse*
%endif


%changelog
* Tue Oct 3 2023 Tom Rix <trix@redhat.com>  - 5.6.0-1
- Initial package
