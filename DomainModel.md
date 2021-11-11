# Domain Model

## Bus Cache Interactions
```
"""
Check if other caches have this address are are waiting for it and return cycles
based on that:
1. Other cache already have valid adress: return cycles equal to 2N where N is number of words
(MESI) or 2 cycles for word (Dragon BusUpd only updates word).
2. Other caches are also waiting for the same data: return min(other cache's blockedFor + time required to
send from that cache to the requesting cache (see option 1), 100)
"""
Bus::cyclesToWait(pid, address) 

"""
Run at the end of the wait period of cache block and looks at below fields:
bool isBlocked{false};
uint32_t blockedFor{};
uint32_t blockedOnAddress{};
CacheOp blockingOperation{CacheOp::PR_RD_MISS};

Based on blockingOperation, issue a bus function to refresh the states for other caches:
P1: E------------------------------------------------> S
P2: I ---wait period---> Cache::issueBusTransaction(): S

And based on the decided states, inserts a new line with a new state in the cache.
"""
Cache::issueBusTransaction()

Cache::prRd(address) ---RdHit--> Cache::block(address, 1) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prRd(address) ---RdMiss--> Bus::cyclesToWait() ---> Cache::block(address, blockedFor, RD_MISS) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prWr(address) ---WrHit--> Cache::block(address, 1) ---wait for those cycles--> Cache::issueBusTransaction()
Cache::prWr(address) ---WrMiss--> Bus::cyclesToWait(address, 1) ---> Cache::block(address, blockedFor, WR_MISS) ---wait for those cycles--> Cache::issueBusTransaction()
```

## System Interactions
```
run () {
  1. advance processors and cache

  # HANDLING BLOCK
  ## Processor.blockedFor > 0 || Processor.cache.blockedFor > 0 (Currently blocked)
  1. Do nothing

  ## Processor.blockedFor == 0 && Processor.cache.blockedFor == 0 (Unblocked)
  1. Check why it was blocked
  2. Blocked for cache state transition -> cache.evictionNeed -> cache.needsEviction() -> Bus::transition() -> System::stateTransition(old block) -> Sytem::blockFor(new block) -> Bus::blockedCycles(processors, pid, CacheOp)
  3. Blocked for cache state transition -> !cache.evictionNeed -> Bus::transition -> stateStateTransition -> System::issueNewInstruction()
  4. Blocked for ALU processor, fetchInstruction()
}

System::issueNewInstruction() {
  # HANDLING NEW INSTRUCTION
  1. Processor.issueNewInstruction() -> set blockingInstruction
  2. New instruction is LD/ST -> cache sets eviction needed and blocks itself -> cache.needsEviction()
  2. New instruction is LD/ST -> cache doesnt need eviction -> System.blockedFor() -> processor.cache.block()
  2. New instruction is ALU -> processor.blockedFor and isBlocked
}
```
## Assumptions:
1. Decode and issue is instant
2. No pipelining
3. Bus transactions after getting the data
4. Intermediate bus transactions can cause short circuiting
5. Snooping on blocknum

