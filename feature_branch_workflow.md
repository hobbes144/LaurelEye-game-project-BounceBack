# Mastering the Feature Branch Workflow in GitHub for C++ Projects

As graduate computer science students, you're likely managing complex C++ projects, whether for coursework, research, or open-source contributions. The **Feature Branch Workflow** is a streamlined GitHub strategy that isolates development tasks, enabling clean collaboration and robust version control. This guide walks you through the workflow with a C++ project example, ensuring your team can develop, test, and merge features like matrix operations or algorithm optimizations without breaking the main codebase.

## Why Use the Feature Branch Workflow?

The Feature Branch Workflow creates a dedicated branch for each new feature, bug fix, or experiment. You develop on this branch and merge it into the `main` branch only after testing and review via GitHub Pull Requests (PRs). For C++ projects, where code changes often involve intricate dependencies or performance tweaks, this approach offers:

- **Isolation**: Test a new matrix multiplication algorithm without risking the stability of the main application.
- **Collaboration**: Multiple students can work on features like a sparse matrix solver or a graph algorithm in parallel.
- **Code Reviews**: Use PRs for peer feedback, refining research or academic code.
- **Reversion**: Easily discard a faulty feature (e.g., a buggy matrix inversion) without affecting the main branch.

## Example: Setting Up a C++ Project

Suppose your team is building `MatrixLib`, a C++ library for matrix operations. Your repository structure might look like this:

```
MatrixLib/
├── include/
│   └── matrix.h
├── src/
│   └── matrix.cpp
├── main.cpp
├── CMakeLists.txt
└── README.md
```

Here’s a sample `main.cpp` to start:

```cpp
#include "include/matrix.h"
#include <iostream>

int main() {
    Matrix m(2, 2);
    m.set(0, 0, 1.0);
    m.set(0, 1, 2.0);
    std::cout << "Matrix value at (0,0): " << m.get(0, 0) << std::endl;
    return 0;
}
```

The `matrix.h` and `matrix.cpp` files define a `Matrix` class with basic getter and setter methods. Your goal is to add a matrix transpose feature using the Feature Branch Workflow.

## Step-by-Step Workflow

### 1. Create a Feature Branch
From the `main` branch, create a branch for your feature. Use a descriptive name, like `user/your-user-name/add-transpose`.

```bash
git checkout main
git pull origin main  # Sync with the remote repository
git checkout -b user/your-user-name/add-transpose
```

### 2. Develop the Feature
Add the transpose functionality to `matrix.h` and `matrix.cpp`. For example, update `matrix.h`:

```cpp
class Matrix {
public:
    Matrix(int rows, int cols);
    double get(int row, int col) const;
    void set(int row, int col, double value);
    Matrix transpose() const;  // New transpose method
private:
    int rows_, cols_;
    std::vector<double> data_;
};
```

And implement it in `matrix.cpp`:

```cpp
Matrix Matrix::transpose() const {
    Matrix result(cols_, rows_);
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            result.set(j, i, get(i, j));
        }
    }
    return result;
}
```

Update `main.cpp` to test the feature:

```cpp
#include "include/matrix.h"
#include <iostream>

int main() {
    Matrix m(2, 2);
    m.set(0, 0, 1.0);
    m.set(0, 1, 2.0);
    m.set(1, 0, 3.0);
    m.set(1, 1, 4.0);
    Matrix t = m.transpose();
    std::cout << "Transposed value at (0,1): " << t.get(0, 1) << std::endl;  // Should print 3.0
    return 0;
}
```

### 3. Commit and Push
Commit your changes with a clear message and push to GitHub:

```bash
git add .
git commit -m "Add matrix transpose functionality"
git push origin user/your-user-name/add-transpose
```

### 4. Create a Pull Request
On GitHub, navigate to your repository. You’ll see a prompt to create a Pull Request for `user/your-user-name/add-transpose`. Open the PR, describe the changes (e.g., “Added matrix transpose method with tests”), and assign teammates for review. Use GitHub’s interface to discuss code, suggest edits, or request changes.

### 5. Merge the Feature
Once approved, merge the PR into `main` using GitHub’s “Merge Pull Request” button. Choose a merge strategy (e.g., “Create a merge commit” for a clear history). Delete the feature branch afterward to keep the repository tidy.

```bash
git checkout main
git pull origin main  # Update local main with the merged changes
```

### 6. Handle Conflicts (If Any)
If another teammate’s feature (e.g., `feature/add-determinant`) causes a merge conflict, GitHub will notify you during the PR. Resolve conflicts locally:

```bash
git checkout user/your-user-name/add-transpose
git merge main
# Resolve conflicts in your editor, then:
git add .
git commit
git push origin user/your-user-name/add-transpose
```

The PR will update, and you can proceed with the merge.

## Best Practices for C++ Projects

- **Test Locally**: Use Visual Studio to build and test your C++ code before pushing.
- **Small Commits**: Break features into small, logical commits (e.g., “Add transpose method” and “Add affine animation code for transpose” separately).
- **Clear PRs**: In your PR description, note what you changed, why, and how you tested it (e.g., “Verified transpose with 2x2 and 3x3 matrices”).
- **Branch Naming**: 
  - Prefixing the branch name with `user/your-user-name` is a common best-practice that helps with repository management and historical analysis.
  - Use prefixes like `feature/`, `bugfix/`, or `experiment/` for clarity (e.g., `user/your-user-name/feature/sparse-matrix-support`).
- **Automate Testing**: Set up GitHub Actions to run `CMake` builds and tests automatically on each push. (this option is not available in GAM550/GAM551)

## Why It Matters for Grad Students

In graduate-level C++ projects, where you might be implementing complex algorithms (e.g., machine learning kernels or graph optimizations), the Feature Branch Workflow ensures your experiments don’t destabilize shared code. It fosters collaboration with peers, aligns with open-source contribution practices, and prepares you for industry-standard workflows.

By mastering this workflow, you’ll keep your `MatrixLib` project organized, scalable, and ready for contributions—whether you’re adding matrix inversions, parallelizing computations, or debugging edge cases. Happy coding!
