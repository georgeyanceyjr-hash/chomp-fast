# V10 candidate implementation notes

This candidate starts from the validated V9 source and retains three measured changes. Exact position membership still uses the existing hash tables. Move order, candidate order, batch size, worker count and class traversal order are preserved.

## Stop corner queries above the maximum stored key

The existing corner enumeration produces strictly increasing child keys. For consecutive eligible zero bits a < b under the same selected one, let E be the first edge value and s the suffix below a. Every intervening bit is one, so the suffix below b is s+b-2a. The child-key difference is (2E-b+s+b-2a)-(E-a+s)=E-a>0. Prefix bits occupy disjoint higher positions. Across selected-one groups, every later child preserves the earlier selected bit, while earlier children clear it. Once a child exceeds the largest key in its class, all remaining children are also too large. The cutoff is disabled for the database fallback without a supplied shard. Minimum-key/floor behavior is unchanged.

## Defer conjugate insertion until the class ends

For a class (w,h) with w>h, its transposes belong to (h,w). All moves weakly decrease both dimensions, so no query in the current class can reach a newly inserted transpose whose height is w>h. Original positions are inserted immediately in ascending order; transposes are inserted before writing the class checkpoint or starting another class. Every discovered transpose is queued even if its original was preloaded already, preserving asymmetric preload behavior. Successful additions are compacted in place into the transpose delta. This avoids an extra full-size temporary queue.

## Merge checkpoint output

Original new keys already arrive sorted. Only the transpose keys need sorting. Two merge passes produce exactly the original sorted stream: the first computes the CHOMPCP2 checksum and the second writes through a 1 MiB buffer. No complete merged vector is allocated. Header format, checksums, class order and resume cursor are unchanged. The original checkpoint writer remains in the source as the reference for the serializer test.

The bounds on correctness here concern complete class checkpoints. These changes do not add recovery from a partially written checkpoint record.
