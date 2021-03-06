
#include "hmac.h"
#include <cfg/test.h>
#include <cfg/debug.h>
#include <sec/hash/sha1.h>
#include <sec/hash/md5.h>
#include <string.h>

int hmac_testSetup(void)
{
	kdbg_init();
	return 0;
}

int hmac_testTearDown(void)
{
	return 0;
}

struct Test_HMAC
{
	const char *key;
	size_t key_len;
	const char *data;
	size_t data_len;
	const char *digest;
};

const struct Test_HMAC tests_hmac_md5[] =
{
	{
		"\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b", 16,
		"Hi There", 8,
		"\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d",
	},
	{
		"Jefe", 4,
		"what do ya want for nothing?", 28,
		"\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7\x38",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 16,
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd", 50,
		"\x56\xbe\x34\x52\x1d\x14\x4c\x88\xdb\xb8\xc7\x33\xf0\xe8\xb3\xf6",
	},
	{
		"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
		"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 25,
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd", 50,
		"\x69\x7e\xaf\x0a\xca\x3a\x3a\xea\x3a\x75\x16\x47\x46\xff\xaa\x79",
	},
	{
		"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c", 16,
		"Test With Truncation", 20,
		"\x56\x46\x1e\xf2\x34\x2e\xdc\x00\xf9\xba\xb9\x95\x69\x0e\xfd\x4c",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 80,
		"Test Using Larger Than Block-Size Key - Hash Key First", 54,
		"\x6b\x1a\xb7\xfe\x4b\xd7\xbf\x8f\x0b\x62\xe6\xce\x61\xb9\xd0\xcd",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 80,
		"Test Using Larger Than Block-Size Key and Larger Than One "
		"Block-Size Data", 73,
		"\x6f\x63\x0f\xad\x67\xcd\xa0\xee\x1f\xb1\xf5\x62\xdb\x3a\xa5\x3e",
	}
};

const struct Test_HMAC tests_hmac_sha1[] =
{
	{
		"\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b", 20,
		"Hi There", 8,
		"\xb6\x17\x31\x86\x55\x05\x72\x64\xe2\x8b\xc0\xb6\xfb\x37\x8c\x8e\xf1\x46\xbe\x00",
	},
	{
		"Jefe", 4,
		"what do ya want for nothing?", 28,
		"\xef\xfc\xdf\x6a\xe5\xeb\x2f\xa2\xd2\x74\x16\xd5\xf1\x84\xdf\x9c\x25\x9a\x7c\x79",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 20,
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
		"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd", 50,
		"\x12\x5d\x73\x42\xb9\xac\x11\xcd\x91\xa3\x9a\xf4\x8a\xa1\x7b\x4f\x63\xf1\x75\xd3",
	},
	{
		"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 25,
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
		"\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd", 50,
		"\x4c\x90\x07\xf4\x02\x62\x50\xc6\xbc\x84\x14\xf9\xbf\x50\xc8\x6c\x2d\x72\x35\xda",
	},
	{
		"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c", 20,
		"Test With Truncation", 20,
		"\x4c\x1a\x03\x42\x4b\x55\xe0\x7f\xe7\xf2\x7b\xe1\xd5\x8b\xb9\x32\x4a\x9a\x5a\x04",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 80,
		"Test Using Larger Than Block-Size Key - Hash Key First", 54,
		"\xaa\x4a\xe5\xe1\x52\x72\xd0\x0e\x95\x70\x56\x37\xce\x8a\x3b\x55\xed\x40\x21\x12",
	},
	{
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
		"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 80,
		"Test Using Larger Than Block-Size Key and Larger "
        "Than One Block-Size Data", 73,
        "\xe8\xe9\x9d\x0f\x45\x23\x7d\x78\x6d\x6b\xba\xa7\x96\x5c\x78\x08\xbb\xff\x1a\x91",
   },
};

static void algo_run_tests(Mac *mac, const struct Test_HMAC *t, int count)
{
	for (int i=0; i<count; ++i, ++t)
	{
		mac_set_key(mac, (const uint8_t*)t->key, t->key_len);
		mac_begin(mac);
		mac_update(mac, (const uint8_t*)t->data, t->data_len);
		ASSERT(memcmp(mac_final(mac), t->digest, mac_digest_len(mac)) == 0);
	}
}

int hmac_testRun(void)
{
	algo_run_tests(hmac_stackinit(MD5_stackinit()),
				   tests_hmac_md5, countof(tests_hmac_md5));

	algo_run_tests(hmac_stackinit(SHA1_stackinit()),
				   tests_hmac_sha1, countof(tests_hmac_sha1));

	return 0;
}

TEST_MAIN(HMAC);
