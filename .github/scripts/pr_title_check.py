#!/usr/bin/env python3
"""Validate pull request title against project naming rules."""

from __future__ import annotations

import os
import string
import sys

ALLOWED_TYPES = {
    "build",
    "chore",
    "ci",
    "docs",
    "feat",
    "fix",
    "perf",
    "refactor",
    "revert",
    "style",
    "test",
}
SCOPE_CHARS = set(string.ascii_letters + string.digits + "_.-")


def fail(title: str, message: str) -> None:
    print(f"::error title={title}::{message}")
    sys.exit(1)


def validate_title(pr_title: str) -> None:
    # Expected format: <type>[(<scope>)][!]: <short summary>
    if ": " not in pr_title:
        fail(
            "Invalid pull request title",
            f"PR title '{pr_title}' does not match '<type>(<scope>)!: <short summary>'.",
        )

    head, summary = pr_title.split(": ", 1)

    # Conventional Commits breaking change indicator, e.g. 'feat!' or 'feat(api)!'.
    if head.endswith("!"):
        head = head[:-1]

    if not summary:
        fail("Invalid pull request summary", "Summary must not be empty.")

    if not summary[0].islower():
        fail(
            "Invalid pull request summary",
            "Summary must start with a lowercase letter.",
        )

    if summary.endswith("."):
        fail(
            "Invalid pull request summary",
            "Summary must not end with a period.",
        )

    if "(" in head or ")" in head:
        if not head.endswith(")"):
            fail(
                "Invalid pull request title",
                "Invalid scope format. Expected '<type>(<scope>)'.",
            )

        open_idx = head.find("(")
        close_idx = head.rfind(")")

        if open_idx <= 0 or close_idx != len(head) - 1:
            fail(
                "Invalid pull request title",
                "Invalid scope placement. Expected '<type>(<scope>)'.",
            )

        title_type = head[:open_idx]
        scope = head[open_idx + 1 : close_idx]

        if not scope:
            fail("Invalid pull request title", "Scope must not be empty.")

        # Allow multiple comma-separated scopes, e.g. (a.c, b.c).
        for sub_scope in scope.split(","):
            sub_scope = sub_scope.strip()

            if not sub_scope:
                fail("Invalid pull request title", "Each scope must not be empty.")

            for ch in sub_scope:
                if ch not in SCOPE_CHARS:
                    fail(
                        "Invalid pull request title",
                        "Scope can only contain letters, digits, '_', '.', and '-' "
                        "(use ', ' to separate multiple scopes).",
                    )
    else:
        title_type = head

    if title_type not in ALLOWED_TYPES:
        fail(
            "Invalid pull request title",
            "Allowed types: build, chore, ci, docs, feat, fix, perf, refactor, "
            "revert, style, test.",
        )


def main() -> None:
    pr_title = os.environ.get("PR_TITLE", "")
    validate_title(pr_title)
    print("PR title follows the required naming convention.")


if __name__ == "__main__":
    main()
