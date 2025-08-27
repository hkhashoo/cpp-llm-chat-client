# Design — C++ LLM Chat Client

This document describes the intended architecture and behavior of the project.  
It’s concise by design; the **diagrams PDF** provides visual detail.

**➡ Download the diagrams:** [design_diagrams_v2.pdf](design_diagrams_v2.pdf)

---

## Overview

A small, portable C++20 command-line chat client that talks to an LLM backend (OpenAI / Hugging Face / mock).  
Core responsibilities are split into CLI, Chat Session, HTTP client, and optional Cache/Logger.

---

## Components & Responsibilities

- **CLI (main.cpp)** — REPL loop, input/output, basic UX.
- **Chat Session** — Holds recent history, builds prompts, returns replies.
- **HTTP Client** — Sends requests, parses responses, handles streaming.
- **Cache (optional)** — SQLite/Redis for prompt/response caching.
- **Logger (optional)** — spdlog for structured logs, request/response metadata.

---

## Interaction Flow (One Turn)

1. User types text in CLI.  
2. Chat appends to history and builds a prompt (last N turns).  
3. Check cache; if hit, return cached response.  
4. If miss, send request over HTTP; parse JSON reply.  
5. Save to cache; print reply to console.  

See **section 2** in the PDF for a numbered diagram.

---

## Error Handling

- **Timeout/Network/429** → retry with exponential backoff + jitter.  
- **401/403** → surface an auth error message and keep REPL running.  
- **JSON parse** → show fallback message, log payload for debugging.  

See **section 3** in the PDF.

---

## Data Model (Initial)

- `Message { role: "user" | "assistant", content: std::string }`  
- `History: std::vector<Message>` trimmed to last N messages  
- Request fields: `{ model, messages, temperature, max_tokens, stream }`  
- Response: `{ text, usage?, finish_reason? }`  

---

## Configuration

A small config file or environment variables:

- `PROVIDER=openai|hf|mock`  
- `OPENAI_API_KEY=...`  
- `MODEL=gpt-4o-mini|...`  
- `STREAM=true|false`  
- `HISTORY_LIMIT=5`  

---

## Non‑Functional Goals

- **Portable** (CMake, minimal deps)  
- **Testable** (unit tests around chat + mocks for HTTP)  
- **Observable** (logs for inputs/outputs/errors)  
- **Extensible** (swap providers / add caching without heavy refactor)  

---

## Roadmap (Short)

- v0.1 — CLI + Chat skeleton + Mock HTTP  
- v0.2 — Real HTTP + JSON (single turn)  
- v0.3 — Multi‑turn + config  
- v0.4 — Streaming responses  
- v0.5 — Cache + logging + tests  

---

## Open Questions

- Provider first: OpenAI vs. Hugging Face vs. mock?  
- Token limit handling and truncation strategy  
- Config format: `.env` vs. JSON/YAML  
- Streaming threading model  
- Cache keys and eviction policy  
