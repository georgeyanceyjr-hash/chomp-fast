#!/usr/bin/env python3
"""Merge a CHOMPP01 seed and complete CHOMPCP2 checkpoint into CHOMPP02."""
import argparse
import heapq
import struct

ap = argparse.ArgumentParser()
ap.add_argument("database")
ap.add_argument("checkpoint")
ap.add_argument("output")
ap.add_argument("--dimension", type=int, required=True)
args = ap.parse_args()

with open(args.database, "rb") as source:
    if source.read(8) != b"CHOMPP01":
        raise SystemExit("invalid CHOMPP01 database")
    seed_count, frontier = struct.unpack("<II", source.read(8))
    seed = struct.unpack(f"<{seed_count}I", source.read(seed_count * 4))

records = []
with open(args.checkpoint, "rb") as source:
    if source.read(8) != b"CHOMPCP2":
        raise SystemExit("invalid CHOMPCP2 checkpoint")
    while True:
        marker = source.read(4)
        if not marker:
            break
        if marker != b"CLAS":
            raise SystemExit("invalid class marker")
        w, h = struct.unpack("<HH", source.read(4))
        count, checksum = struct.unpack("<QQ", source.read(16))
        offset = source.tell()
        last = 0
        if count:
            source.seek(offset + (count - 1) * 8)
            last = struct.unpack("<Q", source.read(8))[0]
        source.seek(offset + count * 8)
        records.append((w, h, count, checksum, offset, last))

if not records or records[-1][:2] != (args.dimension, args.dimension - 1):
    raise SystemExit("checkpoint does not reach the requested dimension")

def feed(checksum, value):
    for _ in range(8):
        checksum ^= value & 255
        checksum = (checksum * 1099511628211) & ((1 << 64) - 1)
        value >>= 8
    return checksum

class RecordValues:
    def __init__(self, record):
        self.w, self.h, self.remaining, self.expected, offset, _ = record
        self.source = open(args.checkpoint, "rb")
        self.source.seek(offset)
        self.checksum = 1469598103934665603
        self.checksum = feed(self.checksum, self.w)
        self.checksum = feed(self.checksum, self.h)
        self.checksum = feed(self.checksum, self.remaining)
        self.previous = -1
        self.buffer = ()
        self.index = 0
    def __iter__(self): return self
    def __next__(self):
        if self.index == len(self.buffer):
            if not self.remaining:
                self.source.close()
                if self.checksum != self.expected:
                    raise SystemExit(f"checksum failure in class {self.w}x{self.h}")
                raise StopIteration
            take = min(self.remaining, 8192)
            raw = self.source.read(take * 8)
            if len(raw) != take * 8:
                raise SystemExit(f"truncated class {self.w}x{self.h}")
            self.buffer = struct.unpack(f"<{take}Q", raw)
            self.index = 0
            self.remaining -= take
        value = self.buffer[self.index]
        self.index += 1
        if value <= self.previous:
            raise SystemExit(f"class {self.w}x{self.h} is not strictly sorted")
        self.previous = value
        self.checksum = feed(self.checksum, value)
        return value

added_count = sum(record[2] for record in records)
total = seed_count + added_count
max_added = max(record[5] for record in records)
with open(args.output, "wb") as out:
    out.write(b"CHOMPP02")
    out.write(struct.pack("<QQII", total, max_added, args.dimension, args.dimension))
    out.write(struct.pack(f"<{seed_count}Q", *seed))
    previous = frontier
    buffer = []
    for value in heapq.merge(*(RecordValues(record) for record in records if record[2])):
        if value <= previous:
            raise SystemExit("checkpoint records overlap, are unsorted, or overlap the seed")
        previous = value
        buffer.append(value)
        if len(buffer) == 8192:
            out.write(struct.pack("<8192Q", *buffer)); buffer.clear()
    if buffer:
        out.write(struct.pack(f"<{len(buffer)}Q", *buffer))

print(f"records={len(records)}; final={records[-1][0]}x{records[-1][1]}")
print(f"original={seed_count:,}; added={added_count:,}; total={total:,}")
