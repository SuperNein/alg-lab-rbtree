import argparse
import os
import random
import string


def random_word(rng: random.Random, min_len: int, max_len: int, prefix: str = "") -> str:
    length = rng.randint(min_len, max_len)
    body_len = max(1, length - len(prefix))
    body = "".join(rng.choices(string.ascii_letters, k=body_len))
    return (prefix + body)[:256]


def write_lines(path: str, lines: list[str]) -> None:
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")
    print(f"Generated {path} ({len(lines)} commands)")


def gen_random_mixed(out_dir: str, ops: int) -> None:
    rng = random.Random(42)
    pool = [random_word(rng, 4, 24) for _ in range(ops // 3)]
    lines = []

    for i in range(ops):
        op = rng.choices(["+", "-", "?"], weights=[45, 25, 30])[0]
        w = rng.choice(pool)
        if op == "+":
            lines.append(f"+ {w} {rng.randint(0, 2**64 - 1)}")
        elif op == "-":
            lines.append(f"- {w}")
        else:
            lines.append(w)

        if i > 0 and i % (ops // 4) == 0:
            lines.append("! Save /tmp/rb_bench_mixed.bin")
            lines.append("! Load /tmp/rb_bench_mixed.bin")

    write_lines(os.path.join(out_dir, "01_random.in"), lines)


def gen_sorted(out_dir: str, ops: int) -> None:
    rng = random.Random(101)
    n_keys = ops // 2
    keys = sorted({random_word(rng, 8, 20).lower() for _ in range(n_keys)})
    lines = [f"+ {k} {idx}" for idx, k in enumerate(keys)]
    rng.shuffle(keys)
    lines.extend(keys)
    write_lines(os.path.join(out_dir, "02_sorted.in"), lines)


def gen_long_keys(out_dir: str, ops: int) -> None:
    rng = random.Random(202)
    prefixes = [
        "a" * 180 + "prefixone",
        "a" * 180 + "prefixtwo",
        "b" * 180 + "commonseq",
    ]
    pool = [random_word(rng, 220, 256, rng.choice(prefixes)) for _ in range(ops // 4)]
    lines = []
    for _ in range(ops):
        w = rng.choice(pool)
        if rng.random() < 0.45:
            lines.append(f"+ {w} {rng.randint(0, 10**12)}")
        else:
            lines.append(w)
    write_lines(os.path.join(out_dir, "03_long_keys.in"), lines)


def gen_io_heavy(out_dir: str, entries: int) -> None:
    rng = random.Random(303)
    lines = [f"+ {random_word(rng, 8, 32)} {i}" for i in range(entries)]
    for _ in range(5):
        lines.append("! Save /tmp/rb_bench_io.bin")
        lines.append("! Load /tmp/rb_bench_io.bin")
    write_lines(os.path.join(out_dir, "04_io_heavy.in"), lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", default="benchmarks/data")
    parser.add_argument("--ops", type=int, default=500_000)
    args = parser.parse_args()

    os.makedirs(args.out, exist_ok=True)
    gen_random_mixed(args.out, args.ops)
    gen_sorted(args.out, args.ops)
    gen_long_keys(args.out, args.ops)
    gen_io_heavy(args.out, min(args.ops // 2, 300_000))


if __name__ == "__main__":
    main()
