# DSE-Disabler

DSE-Disabler is a **proof-of-concept (PoC)** Windows utility demonstrating, how Driver Signature Enforcement (DSE) can be temporarily disabled.

This repository is explicitly a PoC for security research. It is intended for experienced security professionals and kernel engineers.

---

## Purpose / Scope

This project is **not** a production tool. Its purpose is to illustrate, at a high level, how DSE can be bypassed during legitimate research activities (PoC development, fuzzing, driver prototyping, vulnerability validation).

---

## Features

- Demonstration: Enable / Disable DSE (PoC)

---

## Build

Requirements:

- Visual Studio 2019 / 2022
- Desktop Development with C++ workload installed

Note: The produced kernel driver in this repository is intended to be loaded using a manual-mapping approach (manual mapper). Example : https://github.com/TheCruZ/kdmapper

Build:

```bash
git clone https://github.com/LAZEROZOR/DSE-Disabler
cd DSE-Disabler
# open the .sln in Visual Studio
# Build → Release x64
