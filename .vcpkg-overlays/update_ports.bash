#!/usr/bin/env bash

set -euo pipefail

# Ensure the script can be called from any directory
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_ROOT=$(cd "$SCRIPT_DIR/../" && pwd)

echo "DEBUG: REPO_ROOT is set to: '$REPO_ROOT'"
cd "$REPO_ROOT"

# Ensure the tmp directory exists
mkdir -p tmp

# Generate the list of submodules dynamically, filtering only those that are CMake-based projects
if ! git submodule foreach --quiet '([ -f CMakeLists.txt ] && echo $path) || true' > tmp/submodule_list.txt; then
    echo "Warning: Some submodules could not be processed. Continuing with the rest." >&2
fi

# Pre-defined map of additional dependencies for specific ports
declare -A PORT_DEPENDENCIES

PORT_DEPENDENCIES[dune-alugrid]="dune-grid"
PORT_DEPENDENCIES[dune-grid]="dune-common dune-geometry"
PORT_DEPENDENCIES[dune-geometry]="dune-common"
PORT_DEPENDENCIES[dune-grid-glue]="dune-grid"
PORT_DEPENDENCIES[dune-istl]="dune-common"
PORT_DEPENDENCIES[dune-localfunctions]="dune-geometry"
PORT_DEPENDENCIES[dune-testtools]="dune-common"

# Define features and their dependencies
declare -A PORT_FEATURES
PORT_FEATURES[dune-grid]="alberta:Support for Alberta grid implementation"

# Define feature dependencies
declare -A FEATURE_DEPENDENCIES
FEATURE_DEPENDENCIES[dune-grid,alberta]="alberta"

create_port_files() {
    local name=$1
    local source_path=$2
    local port_dir=".vcpkg-overlays/ports/$name"

    # Get the git hash of the submodule
    pushd "$source_path" > /dev/null || { echo "Failed to enter directory: $source_path"; return 1; }
    local git_hash=$(git rev-parse HEAD)
    popd > /dev/null

    # Extract project version from CMakeLists.txt (if available)
    local version="0.1.0"  # Default version
    if grep -q "project.*VERSION" "$source_path/CMakeLists.txt"; then
        version=$(grep "project.*VERSION" "$source_path/CMakeLists.txt" | sed -E 's/.*VERSION\s+([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
    fi

    # Prepare dependencies JSON array
    local dependencies='        {
            "name": "vcpkg-cmake",
            "host": true
        },
        {
            "name": "vcpkg-cmake-config",
            "host": true
        }'

    # Add extra dependencies if present in the map
    if [[ -n "${PORT_DEPENDENCIES[$name]:-}" ]]; then
        for dep in ${PORT_DEPENDENCIES[$name]}; do
            dependencies="$dependencies,
        {
            \"name\": \"$dep\"
        }"
        done
    fi

    # Prepare features section if present
    local features=""
    if [[ -n "${PORT_FEATURES[$name]:-}" ]]; then
        features=',
    "features": {'

        # Process each feature
        IFS=';' read -ra feature_list <<< "${PORT_FEATURES[$name]}"
        for feature_desc in "${feature_list[@]}"; do
            # Split feature name and description
            feature_name="${feature_desc%%:*}"
            feature_description="${feature_desc#*:}"

            features="$features
        \"$feature_name\": {
            \"description\": \"$feature_description\""

            # Add feature dependencies if they exist
            if [[ -n "${FEATURE_DEPENDENCIES[$name,$feature_name]:-}" ]]; then
                features="$features,
            \"dependencies\": ["

                # Add each feature dependency
                IFS=' ' read -ra feat_deps <<< "${FEATURE_DEPENDENCIES[$name,$feature_name]}"
                first_dep=true
                for feat_dep in "${feat_deps[@]}"; do
                    if [ "$first_dep" = true ]; then
                        first_dep=false
                    else
                        features="$features,"
                    fi
                    features="$features
                {
                    \"name\": \"$feat_dep\"
                }"
                done

                features="$features
            ]"
            fi

            features="$features
        },"
        done

        # Remove trailing comma and close features object
        features="${features%,}
    }"
    fi

    # Create vcpkg.json
    cat > "$port_dir/vcpkg.json" << EOF
{
    "name": "$name",
    "version": "$version",
    "description": "Local overlay port for $name",
    "homepage": "https://github.com/dune-community/$name",
    "dependencies": [
$dependencies
    ]$features
}
EOF

    # Find license file
    local license_file=""
    for file in LICENSE.md LICENSE COPYING COPYRIGHT LICENSE.txt COPYING.txt; do
        if [ -f "$source_path/$file" ]; then
            license_file="$file"
            break
        fi
    done

    # Create portfile.cmake
    cat > "$port_dir/portfile.cmake" << EOF
set(VCPKG_BUILD_TYPE release)

vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL "\${CMAKE_CURRENT_LIST_DIR}/../../../deps/$name"
    REF $git_hash
)

vcpkg_cmake_configure(
    SOURCE_PATH "\${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTING=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/$name)

file(REMOVE_RECURSE "\${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "\${CURRENT_PACKAGES_DIR}/debug/share")

EOF

    # Add license installation if a license file was found
    if [ -n "$license_file" ]; then
        cat >> "$port_dir/portfile.cmake" << EOF
file(
    INSTALL "\${SOURCE_PATH}/$license_file"
    DESTINATION "\${CURRENT_PACKAGES_DIR}/share/\${PORT}"
    RENAME copyright
)
EOF
    else
        # If no license file was found, create a placeholder
        cat >> "$port_dir/portfile.cmake" << EOF
# No license file found, creating a placeholder
file(WRITE "\${CURRENT_PACKAGES_DIR}/share/\${PORT}/copyright" "No license file found in the source repository. Please check the source code for license information.")
EOF
    fi

    echo "Created overlay port for $name"
}

# Process all submodules
while read submodule_dir; do
    submodule_name=$(basename "$submodule_dir")

    # Create the overlay port directory
    mkdir -p .vcpkg-overlays/ports/$submodule_name

    create_port_files "$submodule_name" "$submodule_dir"
    pre-commit run cmake-format --files .vcpkg-overlays/ports/$submodule_name/* || true
    pre-commit run clang-format --files .vcpkg-overlays/ports/$submodule_name/* || true
done < tmp/submodule_list.txt
