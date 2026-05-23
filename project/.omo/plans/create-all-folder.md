# Create `all` Folder

## TL;DR
> **Quick Summary**: 在 `my_project` 同级创建空文件夹 `all`，为后续推送两个文件夹到 GitHub 做准备。
>
> **Deliverables**:
> - `\\192.168.11.128\EX_DISK_A\all/` — 空文件夹
>
> **Estimated Effort**: Quick（< 1 分钟）
> **Parallel Execution**: N/A（单任务）

---

## Context

### Original Request
用户需要在 `my_project` 同级创建 `all` 文件夹，方便后面将两个文件夹推送到 GitHub 上。

### Confirmed
- 只创建空文件夹，不移动/复制 `my_project` 内容

---

## TODOs

- [ ] 1. 在 `\\192.168.11.128\EX_DISK_A\` 下创建 `all` 文件夹

  **What to do**:
  - 执行 `mkdir` 创建 `\\192.168.11.128\EX_DISK_A\all` 目录
  - 验证目录创建成功

  **Recommended Agent Profile**: `quick`

  **Acceptance Criteria**:
  - `Test-Path -LiteralPath "\\192.168.11.128\EX_DISK_A\all"` 返回 `True`
  - 目录为空目录

---

## Success Criteria
- [ ] `all` 文件夹存在于 `\\192.168.11.128\EX_DISK_A\all`
- [ ] 文件夹为空，`my_project` 保持不变
