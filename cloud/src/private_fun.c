/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#ifdef CLOUD_PRIVATE_SUPPORT
const char private_ca_crt[] = \
{
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDfzCCAmegAwIBAgIEZ4aQEDANBgkqhkiG9w0BAQsFADBvMQswCQYDVQQGEwJV\r\n" \
"UzELMAkGA1UECBMCQ0ExCzAJBgNVBAcTAlNGMRQwEgYDVQQKEwtUaGluZ3Nib2Fy\r\n" \
"ZDEUMBIGA1UECxMLVGhpbmdzYm9hcmQxGjAYBgNVBAMTEWluc3RhbmNlLXhtdzF3\r\n" \
"aHJ0MCAXDTIyMTAxMzA1MzAyNVoYDzIwNTAwMjI3MDUzMDI1WjBvMQswCQYDVQQG\r\n" \
"EwJVUzELMAkGA1UECBMCQ0ExCzAJBgNVBAcTAlNGMRQwEgYDVQQKEwtUaGluZ3Ni\r\n" \
"b2FyZDEUMBIGA1UECxMLVGhpbmdzYm9hcmQxGjAYBgNVBAMTEWluc3RhbmNlLXht\r\n" \
"dzF3aHJ0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAn5K7EE1vu6+P\r\n" \
"nLx5pa+wIuNOTIwXOaHf812DKq6OLsE+An//b18h8E7Ajv/hMFRlN3SpjxGab5dE\r\n" \
"eRUeVCYk8dNoJFdOObr9bUzKtGhLDrvDqldeRLH5zFx6fLTP5U2Fi7SLXjqqh3dP\r\n" \
"rXm8x2eayeQf/61sMtkMaghHlai9FnE/zY04FCAuoGxvoqur7pm82KhRvpnPxva4\r\n" \
"Skkw4qVzSEK4e/Q69ws7WrP1XI5yQym/kIL6TaRHfp3r6NjdxykdM1BxtNkmRYCI\r\n" \
"RxkEuN5E9JO37l58KOKHmVkvdLrkmfTiRNfWJXEAxUWo8cEoN1fP8O03iv9FocIY\r\n" \
"p3dYevSQMwIDAQABoyEwHzAdBgNVHQ4EFgQUTGoEC9JybTQJLeH/wdPOaqIyW40w\r\n" \
"DQYJKoZIhvcNAQELBQADggEBAEk5BC9q61BD+pIQl29fzb9H2Lv9gi/achlVk8om\r\n" \
"xwImMITj+ooLgE+8wGjy9JJVEtf2116oHaxOhGlrL8ZOu8HBvQT5F/TGTaGUT9va\r\n" \
"ABv/OjTFLVr8dPeGEW5q2fOuUCVjXxyB38MIapCeuqL0ciQiPZFL3BiBfDFD5ysl\r\n" \
"fuLxXxhVDg+RwU9vd2xWeDg11ocKWrmrwzjKlatASrCqtZum8kgsoblLn/77DgWt\r\n" \
"86RkTFTUU68oCqSSwEM3bXg+pDKMgsmSGigyUSXfACT8nLTEow/sn8jII5xMswXQ\r\n" \
"NFQpMxkegQesohjYOMAaLIfiTXtKz2q7jGkxU+S5iWvDChA=\r\n" \
"-----END CERTIFICATE-----\r\n"
};

const char private_client_cert[] = \
{
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDfzCCAmegAwIBAgIEcsBW7zANBgkqhkiG9w0BAQsFADBvMQswCQYDVQQGEwJV\r\n" \
"UzELMAkGA1UECBMCQ0ExCzAJBgNVBAcTAlNGMRQwEgYDVQQKEwtUaGluZ3Nib2Fy\r\n" \
"ZDEUMBIGA1UECxMLVGhpbmdzYm9hcmQxGjAYBgNVBAMTEWluc3RhbmNlLXhtdzF3\r\n" \
"aHJ0MCAXDTIyMTAxMzA1MzAzM1oYDzIwNTAwMjI3MDUzMDMzWjBvMQswCQYDVQQG\r\n" \
"EwJVUzELMAkGA1UECBMCQ0ExCzAJBgNVBAcTAlNGMRQwEgYDVQQKEwtUaGluZ3Ni\r\n" \
"b2FyZDEUMBIGA1UECxMLVGhpbmdzYm9hcmQxGjAYBgNVBAMTEWluc3RhbmNlLXht\r\n" \
"dzF3aHJ0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAkLCtWy1jLreN\r\n" \
"QkaQ1Hu0MoFodeLot7rpIDwE/Nyo2Lqrv9pzQ/OpQUffR89wl61AJx7SVgtDFxj4\r\n" \
"c4sNLHY33VJhPnA3wVOwHPT/3UVrOM3df50zV6iq3yeple/uEYI3VuFpQSYomJ46\r\n" \
"cixPbvZl2vGwwhyJmLs/WlsMOGjj3TY6RnqgLY324IjnqhesmqDTCeakajg2Zueq\r\n" \
"l3nw25thegrrZPoVVZkItWxsSS07f8C+61GJkYJD20Tl6k5ZZqbe5tb7BGxCTc6V\r\n" \
"JPfEy8oRyrQCjJslaGbLeUpfFsMK0x3V+FgGtileQFw3LNBrhYS19QW3+ipl60p8\r\n" \
"Sx4JqXPDswIDAQABoyEwHzAdBgNVHQ4EFgQUvIlagRGwDjW682ndIjWo8jw2Mq0w\r\n" \
"DQYJKoZIhvcNAQELBQADggEBABX7mHpf/RS14LnpOnRSbjsX0z3yaNkz+XiSfucI\r\n" \
"uon2wnXM2u1O41a6RqcS4P8miDFqVHSG4fVHNYodiYP0RBfI19PHQST7xzpUqQMj\r\n" \
"ZIMlXcli1fPwuLieMHpWiH9tkuZ18jA9e0YN5r0uIsvjLRDuCEh6ycu13GRT83G+\r\n" \
"QDpoiEUaZlA70C0sgfX0TcILX3zmf6XOr8GAKH/olk6342E2/3wAOwGzGgFTQy81\r\n" \
"MpmHQ0OmT1pVwuN39j6vRaPCgFYGRZeZtWZaxGUurriao+NRIkSUp8rIRo4sICfG\r\n" \
"iUHmyVsvB+xHIN7pjcRGysIQlD3V6c4I1M1+q5fmb5FGTkQ=\r\n" \
"-----END CERTIFICATE-----\r\n"
};

const char private_client_private_key[] = \
{
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIIEogIBAAKCAQEAkLCtWy1jLreNQkaQ1Hu0MoFodeLot7rpIDwE/Nyo2Lqrv9pz\r\n" \
"Q/OpQUffR89wl61AJx7SVgtDFxj4c4sNLHY33VJhPnA3wVOwHPT/3UVrOM3df50z\r\n" \
"V6iq3yeple/uEYI3VuFpQSYomJ46cixPbvZl2vGwwhyJmLs/WlsMOGjj3TY6Rnqg\r\n" \
"LY324IjnqhesmqDTCeakajg2Zueql3nw25thegrrZPoVVZkItWxsSS07f8C+61GJ\r\n" \
"kYJD20Tl6k5ZZqbe5tb7BGxCTc6VJPfEy8oRyrQCjJslaGbLeUpfFsMK0x3V+FgG\r\n" \
"tileQFw3LNBrhYS19QW3+ipl60p8Sx4JqXPDswIDAQABAoIBACwXWsx1GUtMLeSr\r\n" \
"YkkN5UyauL1rxyTt9UqYX6C6kU1yHRJavFNRthspjrXwVcwyxGDneIs15VzthBCZ\r\n" \
"qAsLf1yXVQDU63ijSwCX/Dxugg0OVtgjjFjDEkbsbDY/phl5x+lWyV9MBIT1erP+\r\n" \
"IrqapVFB425Tie6b6ONc6W1rMTzgaymYxqF199faSoTK4oZfiEhOZSURiQPzIoxH\r\n" \
"gXMbXo5fBEjlTwFg86wZUbRqlUl3+vEGDzG724eETTpDI5cxbBQmhyYhKXb6foPL\r\n" \
"Cs976Rw+fvlNoOQYtrOtYxlh6rO3wpEyaQSmIa767+FGCPJmGfdIz5EOjmhrx9tw\r\n" \
"WCLfLTECgYEAxmoNqCi82VSIMJ6Jwcjy814e49kSMmOsL6VE0K20hYm86n+SHOWm\r\n" \
"OZWk34ppg5NeZfgolHRiU83puOqn/2aV0aI2hXlGRVBmGL0iniahOLVdqwVg2dPS\r\n" \
"i7RpdMgy7TXUCSOEzhWklQI2KF//Um/Sb1xdLUi5EVY3zzTVck9mbikCgYEAuq78\r\n" \
"DAaNcTQKOL4WIuWmWobDq40nODQlacBHR8FX76t5b/HVIoMMeKLwe2evJf70cs4M\r\n" \
"1/D6FL86ofYEssB8ex2zqFXOZ28Byjk629RCNVY0UY6uL4Lebs7qFVrKq37GLNse\r\n" \
"iluKOtBjUdNk0QnqXX+Ja4740SDEeqsxytqk5nsCgYAh1qp3mGMEyabka9VgqE3O\r\n" \
"9aldAINa9qbCBXSkBkv4TlJax+D6bqwXvA6/VRLDrtl1mIoLL4Iper+2msaGB0/b\r\n" \
"zcdwogs6/Qg8hd0mjb4HNt8/sI4g+EA7NUya7x3IWNrNqDIZF5XXF+Z2olCgFZQV\r\n" \
"C/KUK4fsjgC8pO0WLrZwaQKBgCRBh8HuI9oweSu7/yCX6U77jVTr/Qn1Vq7LIQzW\r\n" \
"h/WMH/GVzgb93JYxTvMol+OROHhzfBsUOf19hi2eUhtFNufSROoUgBmmfXqA0uWO\r\n" \
"u60QKOkOBNZFXF244c1z7JiR7nsXsz67uITjo+ZoX9QZUOMHufChu8Eh1Abe6LS0\r\n" \
"8gGxAoGALxSZdnAl9sShrJ0Q8GvbRzHoulwrg4yrcySVxWDk5Y9x15xIRG4IoClb\r\n" \
"DAsBdY+B2F+898OkJJeiF4j/HIqw+2d+OCiA2JIwUeN6Z9bf+AONugZ3YNz9FkZU\r\n" \
"f6pfq2Dy5oZI5ueYs8X31q3W01sApHHQlSa/pOdnrYQcIUMM7Ao=\r\n" \
"-----END RSA PRIVATE KEY-----\r\n"
};
#endif
#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include "private_fun.h"
#include "cloud_common.h"
#include "cJSON.h"
#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif
#include "private_ota.h"

char url_str[64] = {0};
char md5_str[64] = {0};

const char mqtt_broker_ca_crt[] = \
{
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDEzCCAfugAwIBAgIBAjANBgkqhkiG9w0BAQsFADA/MQswCQYDVQQGEwJDTjER\r\n" \
"MA8GA1UECAwIaGFuZ3pob3UxDDAKBgNVBAoMA0VNUTEPMA0GA1UEAwwGUm9vdENB\r\n" \
"MB4XDTIwMDUwODA4MDcwNVoXDTMwMDUwNjA4MDcwNVowPzELMAkGA1UEBhMCQ04x\r\n" \
"ETAPBgNVBAgMCGhhbmd6aG91MQwwCgYDVQQKDANFTVExDzANBgNVBAMMBlNlcnZl\r\n" \
"cjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALNeWT3pE+QFfiRJzKmn\r\n" \
"AMUrWo3K2j/Tm3+Xnl6WLz67/0rcYrJbbKvS3uyRP/stXyXEKw9CepyQ1ViBVFkW\r\n" \
"Aoy8qQEOWFDsZc/5UzhXUnb6LXr3qTkFEjNmhj+7uzv/lbBxlUG1NlYzSeOB6/RT\r\n" \
"8zH/lhOeKhLnWYPXdXKsa1FL6ij4X8DeDO1kY7fvAGmBn/THh1uTpDizM4YmeI+7\r\n" \
"4dmayA5xXvARte5h4Vu5SIze7iC057N+vymToMk2Jgk+ZZFpyXrnq+yo6RaD3ANc\r\n" \
"lrc4FbeUQZ5a5s5Sxgs9a0Y3WMG+7c5VnVXcbjBRz/aq2NtOnQQjikKKQA8GF080\r\n" \
"BQkCAwEAAaMaMBgwCQYDVR0TBAIwADALBgNVHQ8EBAMCBeAwDQYJKoZIhvcNAQEL\r\n" \
"BQADggEBAJefnMZpaRDHQSNUIEL3iwGXE9c6PmIsQVE2ustr+CakBp3TZ4l0enLt\r\n" \
"iGMfEVFju69cO4oyokWv+hl5eCMkHBf14Kv51vj448jowYnF1zmzn7SEzm5Uzlsa\r\n" \
"sqjtAprnLyof69WtLU1j5rYWBuFX86yOTwRAFNjm9fvhAcrEONBsQtqipBWkMROp\r\n" \
"iUYMkRqbKcQMdwxov+lHBYKq9zbWRoqLROAn54SRqgQk6c15JdEfgOOjShbsOkIH\r\n" \
"UhqcwRkQic7n1zwHVGVDgNIZVgmJ2IdIWBlPEC7oLrRrBD/X1iEEXtKab6p5o22n\r\n" \
"KB5mN+iQaE+Oe2cpGKZJiJRdM+IqDDQ=\r\n" \
"-----END CERTIFICATE-----\r\n"
};

const char mqtt_broker_client_cert[] = \
{
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDEzCCAfugAwIBAgIBATANBgkqhkiG9w0BAQsFADA/MQswCQYDVQQGEwJDTjER\r\n" \
"MA8GA1UECAwIaGFuZ3pob3UxDDAKBgNVBAoMA0VNUTEPMA0GA1UEAwwGUm9vdENB\r\n" \
"MB4XDTIwMDUwODA4MDY1N1oXDTMwMDUwNjA4MDY1N1owPzELMAkGA1UEBhMCQ04x\r\n" \
"ETAPBgNVBAgMCGhhbmd6aG91MQwwCgYDVQQKDANFTVExDzANBgNVBAMMBkNsaWVu\r\n" \
"dDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMy4hoksKcZBDbY680u6\r\n" \
"TS25U51nuB1FBcGMlF9B/t057wPOlxF/OcmbxY5MwepS41JDGPgulE1V7fpsXkiW\r\n" \
"1LUimYV/tsqBfymIe0mlY7oORahKji7zKQ2UBIVFhdlvQxunlIDnw6F9popUgyHt\r\n" \
"dMhtlgZK8oqRwHxO5dbfoukYd6J/r+etS5q26sgVkf3C6dt0Td7B25H9qW+f7oLV\r\n" \
"PbcHYCa+i73u9670nrpXsC+Qc7Mygwa2Kq/jwU+ftyLQnOeW07DuzOwsziC/fQZa\r\n" \
"nbxR+8U9FNftgRcC3uP/JMKYUqsiRAuaDokARZxVTV5hUElfpO6z6/NItSDvvh3i\r\n" \
"eikCAwEAAaMaMBgwCQYDVR0TBAIwADALBgNVHQ8EBAMCBeAwDQYJKoZIhvcNAQEL\r\n" \
"BQADggEBABchYxKo0YMma7g1qDswJXsR5s56Czx/I+B41YcpMBMTrRqpUC0nHtLk\r\n" \
"M7/tZp592u/tT8gzEnQjZLKBAhFeZaR3aaKyknLqwiPqJIgg0pgsBGITrAK3Pv4z\r\n" \
"5/YvAJJKgTe5UdeTz6U4lvNEux/4juZ4pmqH4qSFJTOzQS7LmgSmNIdd072rwXBd\r\n" \
"UzcSHzsJgEMb88u/LDLjj1pQ7AtZ4Tta8JZTvcgBFmjB0QUi6fgkHY6oGat/W4kR\r\n" \
"jSRUBlMUbM/drr2PVzRc2dwbFIl3X+ZE6n5Sl3ZwRAC/s92JU6CPMRW02muVu6xl\r\n" \
"goraNgPISnrbpR6KjxLZkVembXzjNNc=\r\n" \
"-----END CERTIFICATE-----\r\n"
};

const char mqtt_broker_client_private_key[] = \
{
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIIEpAIBAAKCAQEAzLiGiSwpxkENtjrzS7pNLblTnWe4HUUFwYyUX0H+3TnvA86X\r\n" \
"EX85yZvFjkzB6lLjUkMY+C6UTVXt+mxeSJbUtSKZhX+2yoF/KYh7SaVjug5FqEqO\r\n" \
"LvMpDZQEhUWF2W9DG6eUgOfDoX2milSDIe10yG2WBkryipHAfE7l1t+i6Rh3on+v\r\n" \
"561LmrbqyBWR/cLp23RN3sHbkf2pb5/ugtU9twdgJr6Lve73rvSeulewL5BzszKD\r\n" \
"BrYqr+PBT5+3ItCc55bTsO7M7CzOIL99BlqdvFH7xT0U1+2BFwLe4/8kwphSqyJE\r\n" \
"C5oOiQBFnFVNXmFQSV+k7rPr80i1IO++HeJ6KQIDAQABAoIBAGWgvPjfuaU3qizq\r\n" \
"uti/FY07USz0zkuJdkANH6LiSjlchzDmn8wJ0pApCjuIE0PV/g9aS8z4opp5q/gD\r\n" \
"UBLM/a8mC/xf2EhTXOMrY7i9p/I3H5FZ4ZehEqIw9sWKK9YzC6dw26HabB2BGOnW\r\n" \
"5nozPSQ6cp2RGzJ7BIkxSZwPzPnVTgy3OAuPOiJytvK+hGLhsNaT+Y9bNDvplVT2\r\n" \
"ZwYTV8GlHZC+4b2wNROILm0O86v96O+Qd8nn3fXjGHbMsAnONBq10bZS16L4fvkH\r\n" \
"5G+W/1PeSXmtZFppdRRDxIW+DWcXK0D48WRliuxcV4eOOxI+a9N2ZJZZiNLQZGwg\r\n" \
"w3A8+mECgYEA8HuJFrlRvdoBe2U/EwUtG74dcyy30L4yEBnN5QscXmEEikhaQCfX\r\n" \
"Wm6EieMcIB/5I5TQmSw0cmBMeZjSXYoFdoI16/X6yMMuATdxpvhOZGdUGXxhAH+x\r\n" \
"xoTUavWZnEqW3fkUU71kT5E2f2i+0zoatFESXHeslJyz85aAYpP92H0CgYEA2e5A\r\n" \
"Yozt5eaA1Gyhd8SeptkEU4xPirNUnVQHStpMWUb1kzTNXrPmNWccQ7JpfpG6DcYl\r\n" \
"zUF6p6mlzY+zkMiyPQjwEJlhiHM2NlL1QS7td0R8ewgsFoyn8WsBI4RejWrEG9td\r\n" \
"EDniuIw+pBFkcWthnTLHwECHdzgquToyTMjrBB0CgYEA28tdGbrZXhcyAZEhHAZA\r\n" \
"Gzog+pKlkpEzeonLKIuGKzCrEKRecIK5jrqyQsCjhS0T7ZRnL4g6i0s+umiV5M5w\r\n" \
"fcc292pEA1h45L3DD6OlKplSQVTv55/OYS4oY3YEJtf5mfm8vWi9lQeY8sxOlQpn\r\n" \
"O+VZTdBHmTC8PGeTAgZXHZUCgYA6Tyv88lYowB7SN2qQgBQu8jvdGtqhcs/99GCr\r\n" \
"H3N0I69LPsKAR0QeH8OJPXBKhDUywESXAaEOwS5yrLNP1tMRz5Vj65YUCzeDG3kx\r\n" \
"gpvY4IMp7ArX0bSRvJ6mYSFnVxy3k174G3TVCfksrtagHioVBGQ7xUg5ltafjrms\r\n" \
"n8l55QKBgQDVzU8tQvBVqY8/1lnw11Vj4fkE/drZHJ5UkdC1eenOfSWhlSLfUJ8j\r\n" \
"ds7vEWpRPPoVuPZYeR1y78cyxKe1GBx6Wa2lF5c7xjmiu0xbRnrxYeLolce9/ntp\r\n" \
"asClqpnHT8/VJYTD7Kqj0fouTTZf0zkig/y+2XERppd8k+pSKjUCPQ==\r\n" \
"-----END RSA PRIVATE KEY-----\r\n"
};

int mqtt_broker_get_topic(char *topic_out, int action_in, char *device_id)
{
    char *ptmp_buf = NULL;
    int topic_len = 0;

    if(topic_out == NULL)
    {
        printf("topic_out is NULL\n");
        return -1;
    }
    switch(action_in)
    {
        case MQTT_BROKER_TOPIC_PUBLISH:
            ptmp_buf = "/asr/v1/devices/%s/update/json";break;
        case MQTT_BROKER_TOPIC_SUBSCRIBE_RESULT:
            ptmp_buf = "/asr/v1/devices/%s/result";break;
        case MQTT_BROKER_TOPIC_SUBSCRIBE_CONTROL:
            ptmp_buf = "/asr/v1/devices/%s/command/json";break;
        case MQTT_BROKER_TOPIC_SUBSCRIBE_OTA:
            ptmp_buf = "/asr/v1/devices/%s/ota/json";break;
        case MQTT_BROKER_TOPIC_PUBLISH_PROGERSS:
            ptmp_buf = "/asr/v1/devices/%s/progress/json";break;
        case MQTT_BROKER_TOPIC_SUBSCRIBE_DATA:
            ptmp_buf = "/asr/v1/devices/%s/data";break;
        case MQTT_BROKER_TOPIC_SUBSCRIBE_WILL:{
            ptmp_buf = "/asr/v1/devices/will";
            memcpy(topic_out, ptmp_buf ,strlen(ptmp_buf)+1);
            printf("topic:%s\n", topic_out);
            return 0;}break;
        default:{
            printf("topic input error\n");
            return -1;}break;
    }
    topic_len = strlen(ptmp_buf) + strlen(device_id) + 1;
    snprintf(topic_out, topic_len, ptmp_buf, device_id);
    printf("topic:%s\n", topic_out);
    return 0;
}

int mqtt_broker_get_value(mqtt_broker_para_t *para)
{
    int ret;
    char mqtt_broker_host[129] = {0};
    int32_t mqtt_broker_host_len = 127;
    int mqtt_broker_port = 1883;
    int32_t mqtt_broker_port_len = 4;
    char mqtt_broker_tls_switch[16] = {0};
    int32_t mqtt_broker_tls_switch_len = 15;
    char mqtt_broker_username[64] = {0};
    int32_t mqtt_broker_username_len = 63;
    char mqtt_broker_userpass[64] = {0};
    int32_t mqtt_broker_userpass_len = 63;
    if(para == NULL)
        return -1;
    memset(para, 0, sizeof(mqtt_broker_para_t));

        ret = lega_flash_kv_get("mqtt_broker_host", mqtt_broker_host, &mqtt_broker_host_len);
        if(ret != 0)
        {
            printf("mqtt_broker host get error\n");
            return -1;
        }
        else
        {
            printf("mqtt_broker host = %s\n", mqtt_broker_host);
        }

        ret = lega_flash_kv_get("mqtt_broker_tls_switch", mqtt_broker_tls_switch, &mqtt_broker_tls_switch_len);
        if(ret != 0)
        {
            printf("mqtt_broker_tls_switch get error\n");
            return -1;
        }
        else
        {
            printf("mqtt_broker_tls_switch = %s\n", mqtt_broker_tls_switch);
        }

        ret = lega_flash_kv_get("mqtt_broker_username", mqtt_broker_username, &mqtt_broker_username_len);
        if(ret != 0)
        {
            printf("mqtt_broker_username get error\n");
            return -1;
        }
        else
        {
            printf("mqtt_broker_username = %s\n", mqtt_broker_username);
        }

        ret = lega_flash_kv_get("mqtt_broker_userpass", mqtt_broker_userpass, &mqtt_broker_userpass_len);
        if(ret != 0)
        {
            printf("mqtt_broker_userpass get error\n");
            return -1;
        }
        else
        {
            printf("mqtt_broker_userpass = %s\n", mqtt_broker_userpass);
        }

        ret = lega_flash_kv_get("mqtt_broker_port", &mqtt_broker_port, &mqtt_broker_port_len);
        if(ret != 0)
        {
            printf("mqtt_broker port get error\n");
            mqtt_broker_port = 1883;
        }
        else
        {
            printf("mqtt_broker port = %d\n", mqtt_broker_port);
        }

        strncpy(para->mqtt_broker_host, mqtt_broker_host, mqtt_broker_host_len);
        strncpy(para->mqtt_broker_tls_switch, mqtt_broker_tls_switch, mqtt_broker_tls_switch_len);
        strncpy(para->mqtt_broker_username, mqtt_broker_username, mqtt_broker_username_len);
        strncpy(para->mqtt_broker_userpass, mqtt_broker_userpass, mqtt_broker_userpass_len);
        para->mqtt_broker_port = mqtt_broker_port;
        printf("mqtt_broker_host       = %s\n", para->mqtt_broker_host);
        printf("mqtt_broker_tls_switch = %s\n", para->mqtt_broker_tls_switch);
        printf("mqtt_broker_username   = %s\n", para->mqtt_broker_username);
        printf("mqtt_broker_userpass   = %s\n", para->mqtt_broker_userpass);
        printf("mqtt_broker_port       = %d\n", para->mqtt_broker_port);
        return 0;
}

int parse_mqtt_Command_json(char *data)
{
    cJSON *json, *json_value;

    json = cJSON_Parse(data);
    if(NULL == json)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }

    json_value = cJSON_GetObjectItem(json, "switch");
    if(json_value->type == cJSON_String)
    {
        if(!strcmp(json_value->valuestring,"True"))
        {
            light_flag = LIGHT_ON;
            light_control();
        }
        else  if(!strcmp(json_value->valuestring,"False"))
        {
            light_flag = LIGHT_OFF;
            light_control();
        }
    }

    cJSON_Delete(json);

    return 0;
}

int parse_mqtt_OTA_json(char *data)
{
    cJSON *json, *json_url, *json_ver, *json_md5;
    int new_ver = 0;
    json = cJSON_Parse(data);
    if(NULL == json)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }

    json_url = cJSON_GetObjectItem(json, "otaURL");
    if(json_url->type == cJSON_String)
    {
        memcpy(url_str,json_url->valuestring,strlen(json_url->valuestring));
    }

    json_ver = cJSON_GetObjectItem(json, "newVersion");
    if(json_ver->type == cJSON_Number)
    {
        new_ver = json_ver->valueint;
    }

    json_md5 = cJSON_GetObjectItem(json, "Md5");
    if(json_md5->type == cJSON_String)
    {
        memcpy(md5_str,json_md5->valuestring,strlen(json_md5->valuestring));
    }

    cJSON_Delete(json);

    if(Current_Version >= new_ver)
    {
        printf("do not need upgrade: %d\n",Current_Version);
        return -2;
    }
    else
    {
        if((strlen(url_str)!=0)&&(strlen(md5_str)!=0))
        {
            private_ota_entry();
        }
    }

    return 0;
}

#endif
