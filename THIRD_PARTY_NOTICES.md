### Local integration changes

- Added `aead.c` and `crypto_aead.h` files to TinyJAMBU and Xoodyak libraries as an adapter exposing the `crypto_aead_encrypt()` and `crypto_aead_decrypt()` interface used by the benchmark.
- Adapter code is licensed under the project's MIT License.
- Original upstream copyright and license notices are retained.
- The upstream cryptographic implementations are unchanged.
- Note: The original `aead.c` and `crypto_aead.h` files from ASCON library are unchanged.