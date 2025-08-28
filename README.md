# C++ LLM Chat Client

This is a small project I’m building in C++20 to get hands-on experience with connecting a C++ application to a Large Language Model (LLM) API.  
The idea is to have a simple command-line chat client that can send prompts to an API (like OpenAI or Hugging Face) and display the responses.

---

## Goals

- Set up a clean C++ project using CMake (so it’s easy to build anywhere).  
- Write a chat interface that keeps track of conversation history.  
- Call an LLM API from C++ and show the output.  
- Add streaming responses (printing tokens as they arrive).  
- Cache prompts and responses locally (SQLite or Redis) to avoid repeated API calls.  
- Add proper logging and error handling.  
- Write a few unit tests so the code stays reliable.  

---

## Planned Features

- Command-line chat loop where you type input and get back responses.  
- Multi-turn context (so the model “remembers” the last few messages).  
- Configurable backend (could point to OpenAI, Hugging Face, or a mock).  
- Optionally store conversation history in a local database.  
- Simple logs that show what’s happening internally.  

---

## Project Structure (planned)

```
cpp-llm-chat-client/
├── CMakeLists.txt       # build rules
├── src/                 # .cpp implementation files
├── include/             # .hpp headers
├── cmake/               # extra cmake modules (if needed later)
├── docs/                # design notes
└── tests/               # unit tests
```

---

## Commit Style

I’m using conventional commit prefixes to keep the history clean:

- [chore] for housekeeping (gitignore, configs)  
- [build] for build system changes  
- [feat] for new features  
- [fix] for bug fixes  
- [docs] for documentation  
- [test] for tests  
- [refactor] for non-functional code changes  

### CLI overrides
- `--stream | --no-stream`
- `--model <name>`
- `--history <N>`
- `--timeout <ms>`
