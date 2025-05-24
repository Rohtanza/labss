# CN Lab 12
# Muhammad Rayhan 22P-9106
# Ive selected the 1, 7 and 14, these were the easiest

def longestDistinctSubseq(nums):
    seen = set()
    left = 0
    bestStart, bestLen = 0, 0

    for right, val in enumerate(nums):
        # if there are any duplicate then we will shrink the window from the left
        while val in seen:
            seen.remove(nums[left])
            left += 1
        seen.add(val)

        # updating the best window if larger
        currLen = right - left + 1
        if currLen > bestLen:
            bestStart, bestLen = left, currLen

    return nums[bestStart : bestStart + bestLen]

def firstNonRepeatingChar(s):
    counts = {}
    for ch in s:
        counts[ch] = counts.get(ch, 0) + 1

    for ch in s:
        if counts[ch] == 1:
            return ch
    return None


def rotateRight(nums, k):
    n = len(nums)
    if n == 0:
        return nums

    k %= n
    # slicing the last k elements to front
    return nums[-k:] + nums[:-k]


if __name__ == "__main__":
    data1 = [1, 2, 3, 1, 2, 4, 5]
    print("Longest distinct subseq of", data1, "→", longestDistinctSubseq(data1))

    s1 = "swiss"
    s2 = "aabbcc"
    print(f"First non repeating in '{s1}' →", firstNonRepeatingChar(s1))
    print(f"First non repeating in '{s2}' →", firstNonRepeatingChar(s2))

    data2 = [1, 2, 3, 4]
    steps = 2
    print(f"Rotate {data2} by {steps} →", rotateRight(data2, steps))
