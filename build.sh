#!/bin/bash

# build.sh - Cross-platform build script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect platform
detect_platform() {
    case "$(uname -s)" in
    Darwin*) PLATFORM="macOS" ;;
    Linux*) PLATFORM="Linux" ;;
    CYGWIN* | MINGW* | MSYS*) PLATFORM="Windows" ;;
    *) PLATFORM="Unknown" ;;
    esac
    print_status "Detected platform: $PLATFORM"
}

# Check dependencies
check_dependencies() {
    print_status "Checking dependencies..."

    # Check CMake
    if ! command -v cmake &>/dev/null; then
        print_error "CMake is not installed. Please install CMake 3.16 or later."
        exit 1
    else
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d" " -f3)
        print_success "CMake found: $CMAKE_VERSION"
    fi

    # Check Qt6
    if ! command -v qmake6 &>/dev/null && ! command -v qmake &>/dev/null; then
        print_warning "Qt6 qmake not found in PATH. Make sure Qt6 is installed."
        print_warning "You may need to set Qt6_DIR environment variable."
    else
        print_success "Qt6 found"
    fi

    # Platform-specific checks
    case $PLATFORM in
    "macOS")
        if ! command -v xcodebuild &>/dev/null; then
            print_warning "Xcode command line tools not found. Install with: xcode-select --install"
        fi
        ;;
    "Linux")
        if ! dpkg -l | grep -q libqt6; then
            print_warning "Qt6 development packages may not be installed."
            print_warning "Install with: sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential"
        fi
        ;;
    "Windows")
        print_warning "Make sure you have Visual Studio 2019/2022 or MinGW installed."
        ;;
    esac
}

# Build function
build_project() {
    print_status "Building Calculator..."

    # Create build directory
    if [ -d "build" ]; then
        print_status "Cleaning existing build directory..."
        rm -rf build
    fi

    mkdir build
    cd build

    # Configure
    print_status "Configuring project..."
    case $PLATFORM in
    "macOS")
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15
        ;;
    "Linux")
        cmake .. -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local
        ;;
    "Windows")
        cmake .. -DCMAKE_BUILD_TYPE=Release
        ;;
    *)
        cmake .. -DCMAKE_BUILD_TYPE=Release
        ;;
    esac

    # Build
    print_status "Compiling..."
    cmake --build . --config Release

    print_success "Build completed successfully!"
    cd ..
}

# Package function
package_project() {
    print_status "Creating package..."
    cd build

    case $PLATFORM in
    "macOS")
        # Create DMG
        cpack -G DragNDrop
        print_success "macOS DMG package created in build/ directory"
        ;;
    "Linux")
        # Create DEB and RPM packages
        cpack -G DEB
        cpack -G RPM
        print_success "Linux packages (DEB/RPM) created in build/ directory"
        ;;
    "Windows")
        # Create NSIS installer and ZIP
        cpack -G NSIS
        cpack -G ZIP
        print_success "Windows installer and ZIP created in build/ directory"
        ;;
    esac

    cd ..
}

# Install function
install_project() {
    print_status "Installing Calculator..."
    cd build

    case $PLATFORM in
    "macOS")
        print_status "Installing to Applications folder..."
        sudo cmake --install . --config Release
        print_success "Calculator installed to /Applications/"
        ;;
    "Linux")
        print_status "Installing system-wide..."
        sudo cmake --install . --config Release

        # Update desktop database
        if command -v update-desktop-database &>/dev/null; then
            sudo update-desktop-database /usr/local/share/applications
        fi

        # Update icon cache
        if command -v gtk-update-icon-cache &>/dev/null; then
            sudo gtk-update-icon-cache /usr/local/share/icons/hicolor
        fi

        print_success "Calculator installed to /usr/local/"
        ;;
    "Windows")
        print_warning "On Windows, please run the installer package instead."
        ;;
    esac

    cd ..
}

# Main script
main() {
    print_status "Calculator Build Script"
    print_status "======================="

    detect_platform
    check_dependencies

    case "${1:-build}" in
    "build")
        build_project
        ;;
    "package")
        build_project
        package_project
        ;;
    "install")
        build_project
        install_project
        ;;
    "clean")
        print_status "Cleaning build directory..."
        rm -rf build
        print_success "Build directory cleaned"
        ;;
    "help" | "-h" | "--help")
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  build    - Build the project (default)"
        echo "  package  - Build and create distribution packages"
        echo "  install  - Build and install system-wide"
        echo "  clean    - Clean build directory"
        echo "  help     - Show this help message"
        ;;
    *)
        print_error "Unknown command: $1"
        print_status "Use '$0 help' for usage information"
        exit 1
        ;;
    esac
}

main "$@"
