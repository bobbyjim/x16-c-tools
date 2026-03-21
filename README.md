# x16-c-packages
Code library and resources for the X16

## Package structure and management
The x16-c-packages repository is organized into packages, each containing a library of C and assembly code, tooling, and assets. Each package is defined by a `package.yaml` file that specifies the package name, version, type, and dependencies. The `package.yaml` file also includes metadata about the package, such as a description, author, and license.

    name: text.utext
	version: 1.0.0
	type: hybrid

## Package types

- library: A library of C and assembly code that can be used by other packages. It cannot be executed on its own.
- hybrid: A package that contains both a library and tooling. It can be used by other packages and also executed on its own.
- tool: A package that contains tooling that can be executed on its own. It cannot be used by other packages.
- asset: A package that contains assets that can be used by other packages. It cannot be executed on its own.

## Package structure
Each package should follow the following structure:

```
package.yaml
README.md
src/            (library source code)
include/        (library header files)
test/           (library test code)
tools/          (executable tools)
data/raw/       (raw data)
data/c/         (C data)
data/x16/       (X16 data)
```
