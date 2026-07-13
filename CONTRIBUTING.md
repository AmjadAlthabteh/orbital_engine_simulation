# Contributing

## Before you start

- Search existing issues before opening a new one.
- Keep changes focused on one problem.
- Discuss large features before investing in an implementation.

## Development workflow

1. Create a branch from `master`.
2. Build the project in Debug mode.
3. Make the smallest complete change.
4. Rebuild and test the affected behavior.
5. Use a concise, imperative commit message.

Keep documentation updates in the same pull request when they explain or verify
the code change.

Windows build:

```bat
cd black_hole_travel_timed_1
build.bat --debug
```

## Pull requests

Describe the problem, the solution, and how you verified it. Include screenshots
or recordings for visible changes. Do not commit generated build output, IDE
state, or unrelated formatting changes.
Keep PR descriptions short, but include enough context for someone to reproduce
the changed behavior.
