# Code Formatting Strategy

**Date**: 2025-11-03
**Status**: Incremental adoption (industry best practice)

---

## Overview

Source SDK 2013 is a **legacy codebase** (2013 vintage) with ~1000+ files of Valve's original code. We're building Source 1.5 on top of it, adding new code in `src/framework/` and `tests/`.

**Challenge**: How do we enforce modern code formatting on our new code without disrupting Valve's original SDK?

**Solution**: **Incremental formatting** - check only changed lines in pull requests.

---

## Industry Best Practices (2025)

Based on research from LLVM, Google, Chromium, and the clang-format community:

### ✅ What to DO

1. **Check only changed lines** using `git clang-format --diff`
2. **Use dedicated GitHub Actions** like `jidicula/clang-format-action`
3. **Preserve git history** with `.git-blame-ignore-revs`
4. **Start non-blocking** (warnings), make blocking later
5. **Format incrementally** as you touch files

### ❌ What NOT to do

1. ❌ Format entire codebase at once (breaks git blame)
2. ❌ Check all files in CI (slow, irrelevant for legacy code)
3. ❌ Mix formatting changes with logic changes
4. ❌ Format code you didn't touch

---

## Our Implementation

### CI Workflow

We use **`jidicula/clang-format-action@v4.13.0`** which:
- Checks **only the lines you changed** in your PR
- Uses `git clang-format --diff` under the hood
- Works on Ubuntu (faster than Windows)
- Industry-standard action (widely used)

```yaml
format-check:
  name: Check formatting (changed lines only)
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v4
      with:
        fetch-depth: 0  # Need full history for git diff

    - name: Check formatting of changed lines
      uses: jidicula/clang-format-action@v4.13.0
      with:
        clang-format-version: '18'
        check-path: 'src'
        fallback-style: 'file'
```

**What this does:**
- Compares your branch to the base branch (master)
- Checks formatting **only on lines you modified**
- Uses `.clang-format` configuration file
- Fails PR if your changes aren't formatted

**What this does NOT do:**
- ✅ Doesn't touch Valve's original SDK code
- ✅ Doesn't reformat entire files
- ✅ Doesn't check unchanged lines in your files

---

## Git Blame Protection

When we do bulk formatting (rare), we add commits to `.git-blame-ignore-revs`:

```bash
# Configure git to use this file locally
git config blame.ignoreRevsFile .git-blame-ignore-revs
```

GitHub automatically respects this file in the web UI.

**Current ignored commits:**
- `dc1b9870` - Initial bulk formatting of framework/ and tests/ (2025-11-03)

---

## Developer Workflow

### When Writing New Code

**Option 1: Format before commit (recommended)**
```bash
# Format only your changes
git add -u
git clang-format

# Review the formatting changes
git diff

# Commit everything together
git add -A
git commit -m "feat: add new feature"
```

**Option 2: Let CI tell you**
```bash
# Commit your changes
git commit -m "feat: add new feature"
git push

# CI will tell you if formatting is needed
# If it fails, run:
git clang-format origin/master
git commit --amend --no-edit
git push --force-with-lease
```

### When Modifying Existing Code

If you touch a file that wasn't previously formatted:
```bash
# Only format the lines you changed
git clang-format origin/master

# Don't format the whole file unless you're committing to it
```

### Pre-commit Hook (Optional)

Create `.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Auto-format staged changes before commit
git clang-format --staged --quiet
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

---

## Formatting Configuration

File: `.clang-format`

**Style**: K&R (Kernighan & Ritchie) with Valve conventions
- **Indentation**: Tabs (4 spaces wide)
- **Braces**: Attach style (same line)
- **Pointers**: Left-aligned (`char* ptr`)
- **Line length**: 120 characters
- **Includes**: Preserve order (don't sort)

**Why K&R?** Matches Valve's existing SDK code style.

---

## Incremental Adoption Phases

### Phase 1: Current (Nov 2025)
- ✅ CI checks changed lines only
- ✅ Non-blocking (warnings only)
- ✅ New code in `src/framework/` and `tests/` formatted
- ⏳ Developers learn to use `git clang-format`

### Phase 2: Near-term (Dec 2025)
- Make formatting checks **blocking** (fail PRs)
- Add pre-commit hook to project template
- Format one major directory (e.g., `src/game/client/hl2mp/`)
- Add commit to `.git-blame-ignore-revs`

### Phase 3: Medium-term (Q1 2026)
- Format other directories as we touch them
- Expand checks to more file types
- Add clang-tidy checks (static analysis)

### Phase 4: Long-term (Q2 2026+)
- Entire codebase consistently formatted
- Formatting is automatic and transparent
- Focus shifts to other code quality metrics

---

## Why This Approach?

### Advantages
- ✅ **Preserves git history** - blame still works on old code
- ✅ **Gradual improvement** - codebase gets better over time
- ✅ **No big-bang disruption** - no massive commits
- ✅ **Developer-friendly** - clear, actionable errors
- ✅ **Industry standard** - proven by LLVM, Google, etc.

### Alternatives Considered

**Alternative 1: Format everything at once**
- ❌ Massive git commit (hard to review)
- ❌ Breaks git blame
- ❌ Merge conflicts for open PRs
- ❌ Disrupts workflow

**Alternative 2: Never format anything**
- ❌ Code quality degrades
- ❌ Inconsistent style
- ❌ Harder to read/maintain

**Alternative 3: Format only new files**
- ⚠️ Mixed formatting in modified files
- ⚠️ Hard to enforce boundaries
- ⚠️ Confusing for developers

---

## Tools Used

### clang-format
- **Version**: 18 (matches GitHub Actions)
- **Install (Windows)**: `choco install llvm`
- **Install (Linux)**: `sudo apt-get install clang-format`
- **Install (macOS)**: `brew install clang-format`

### git-clang-format
- Included with clang-format
- Formats only changed lines based on git diff
- Industry standard for incremental formatting

### GitHub Actions
- **jidicula/clang-format-action@v4.13.0**
- Mature, widely-used action
- Built-in support for changed-line checking
- Fast (runs on Ubuntu)

---

## FAQ

**Q: Why not use clang-format on the entire codebase?**
A: Valve's original code (2013) has its own style. Reformatting it creates massive, hard-to-review commits and breaks git blame.

**Q: What if I need to format an entire file?**
A: That's fine! Just make it a separate commit:
```bash
clang-format -i src/framework/myfile.cpp
git commit -m "style: format myfile.cpp"
```

**Q: Does this slow down CI?**
A: No, checking only changed lines is very fast (typically <10 seconds).

**Q: What if clang-format version changes?**
A: We pin to version 18 in the workflow. Local version differences are acceptable for incremental formatting.

**Q: Can I format Valve's SDK code?**
A: You *can*, but please don't. Keep it in a separate commit with rationale, and add to `.git-blame-ignore-revs`.

**Q: What about header files included from both old and new code?**
A: Format headers incrementally. If a header is mostly unchanged, only format the lines you modify.

---

## References

- LLVM Coding Standards: https://llvm.org/docs/CodingStandards.html
- clang-format documentation: https://clang.llvm.org/docs/ClangFormat.html
- git-clang-format: https://github.com/llvm/llvm-project/blob/main/clang/tools/clang-format/git-clang-format
- jidicula/clang-format-action: https://github.com/jidicula/clang-format-action
- Research: "Enforce code consistency with clang-format" (Red Hat, 2022)

---

## Summary

**Incremental formatting is the industry-standard approach for legacy codebases.**

- Check only changed lines in PRs
- Preserve git history
- Gradual improvement over time
- Developer-friendly

**Next steps:**
1. Developers: Use `git clang-format` before pushing
2. Week 2: Make formatting checks blocking
3. Month 1: Format first major directory
4. Quarter 1: Expand to more directories

---

*Last updated: 2025-11-03*
*Status: Incremental adoption active, CI enforcing changed-line checks*
