/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  이 소프트웨어와 관련 문서 파일(이하 "소프트웨어")을 받는 모든 사람에게 무료로 소프트웨어를 사용할 수 있는 권한을 부여합니다. 
  소프트웨어를 제한 없이 사용할 수 있으며, 복사, 수정, 병합, 출판, 배포, 서브라이선스, 판매할 수 있습니다. 
  또한 소프트웨어가 제공되는 대로 제공되므로 어떠한 보증도 없으며, 소프트웨어 사용으로 인한 모든 책임은 사용자에게 있습니다.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* 
윈도우용으로 컴파일할 때, 기본 호출 규약이 다른 프로젝트에서 호출될 때 발생할 수 있는 문제를 피하기 위해 특정 호출 규약을 지정합니다. 
윈도우의 경우 3가지 정의 옵션이 있습니다:

CJSON_HIDE_SYMBOLS - 기호를 절대로 dllexport하지 않으려는 경우 정의
CJSON_EXPORT_SYMBOLS - 라이브러리 빌드 시 기호를 dllexport하려는 경우 정의 (기본값)
CJSON_IMPORT_SYMBOLS - 기호를 dllimport하려는 경우 정의

*유닉스 계열 빌드에서 가시성 속성을 지원하는 경우, 비슷한 동작을 정의하려면 다음을 수행할 수 있습니다:

- `-fvisibility=hidden` (gcc용) 또는 `-xldscope=hidden` (sun cc용)을 CFLAGS에 추가하여 기본 가시성을 숨김으로 설정
- 그런 다음 `CJSON_API_VISIBILITY` 플래그를 사용하여 CJSON_EXPORT_SYMBOLS와 동일한 방식으로 기호를 "export" 합니다.
*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* 기본적으로 기호를 export하도록 설정, 이는 C와 헤더 파일을 복사해서 사용할 때 필요합니다 */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* 프로젝트 버전 */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 18

#include <stddef.h>

/* cJSON 타입들: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* 원시 JSON */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* cJSON 구조체: */
typedef struct cJSON
{
    /* next/prev를 통해 배열/객체 체인을 순회할 수 있습니다. 또는 GetArraySize/GetArrayItem/GetObjectItem을 사용할 수 있습니다. */
    struct cJSON *next;
    struct cJSON *prev;
    /* 배열 또는 객체 항목은 배열/객체의 항목 체인을 가리키는 child 포인터를 가집니다. */
    struct cJSON *child;

    /* 항목의 타입, 위에서 정의한 대로 */
    int type;

    /* 타입이 cJSON_String 및 cJSON_Raw인 경우 항목의 문자열 */
    char *valuestring;
    /* valueint에 쓰는 것은 더 이상 권장되지 않으며, 대신 cJSON_SetNumberValue를 사용하세요 */
    int valueint;
    /* 타입이 cJSON_Number인 경우 항목의 숫자 */
    double valuedouble;

    /* 이 항목이 객체의 자식이거나 객체의 하위 항목 목록에 있는 경우 항목의 이름 문자열 */
    char *string;
} cJSON;

/* FreeRTOS 호환성을 위해 realloc_fn을 포함하도록 cJSON_Hooks 구조체를 확장 */
typedef struct cJSON_Hooks
{
      /* malloc/free는 컴파일러의 기본 호출 규약과 상관없이 Windows에서 항상 CDECL입니다. 따라서 hooks가 이러한 함수를 직접 전달할 수 있도록 해야 합니다. */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
      void *(CJSON_CDECL *realloc_fn)(void *ptr, size_t size); /* FreeRTOS 호환성을 위해 realloc_fn 추가 */
} cJSON_Hooks;

typedef int cJSON_bool;

/* cJSON이 파싱을 거부하기 전에 배열/객체가 얼마나 깊게 중첩될 수 있는지 제한합니다.
 * 이는 스택 오버플로우를 방지하기 위한 것입니다. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* cJSON이 파싱을 거부하기 전에 순환 참조의 길이를 제한합니다.
 * 이는 스택 오버플로우를 방지하기 위한 것입니다. */
#ifndef CJSON_CIRCULAR_LIMIT
#define CJSON_CIRCULAR_LIMIT 10000
#endif

/* cJSON의 버전을 문자열로 반환 */
CJSON_PUBLIC(const char*) cJSON_Version(void);

/* malloc, realloc 및 free 함수를 cJSON에 제공 */
CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);

/* 메모리 관리: 호출자는 항상 cJSON_Parse의 모든 변형 결과(cJSON_Delete 사용)와 cJSON_Print(cJSON_Delete, cJSON_Hooks.free_fn 또는 cJSON_free 사용)에 대한 결과를 해제할 책임이 있습니다.
 * 예외는 cJSON_PrintPreallocated으로, 호출자가 버퍼에 대한 모든 책임을 집니다. */
/* JSON 블록을 제공하면, 이를 통해 조사할 수 있는 cJSON 객체를 반환합니다. */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts는 JSON이 null로 종료되었는지 요구하고 확인하며, 파싱이 끝난 최종 바이트에 대한 포인터를 검색할 수 있게 해줍니다.
 * return_parse_end에 포인터를 제공하면, 파싱이 실패할 경우 return_parse_end는 에러 포인터를 포함하게 되어 cJSON_GetErrorPtr()과 일치합니다. */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

/* cJSON 엔티티를 전송/저장을 위해 텍스트로 렌더링 */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);
/* 포맷 없이 cJSON 엔티티를 전송/저장을 위해 텍스트로 렌더링 */
CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);
/* 버퍼 전략을 사용하여 cJSON 엔티티를 텍스트로 렌더링. prebuffer는 최종 크기에 대한 추측입니다. 잘 추측하면 재할당이 줄어듭니다. fmt=0은 비포맷, =1은 포맷 */
CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
/* 이미 메모리에 할당된 버퍼를 사용하여 cJSON 엔티티를 텍스트로 렌더링합니다. 성공 시 1, 실패 시 0을 반환합니다.
 * NOTE: cJSON은 필요한 메모리를 100% 정확하게 추정하지 못하므로 실제로 필요한 것보다 5바이트 더 할당하는 것이 안전합니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
/* cJSON 엔티티와 모든 하위 엔티티를 삭제합니다. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);

/* 배열(또는 객체)에 있는 항목의 수를 반환합니다. */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);
/* 배열 "array"에서 인덱스 "index"에 있는 항목을 검색합니다. 실패 시 NULL을 반환합니다. */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);
/* 객체에서 "string" 항목을 가져옵니다. 대소문자 구분하지 않음. */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);
/* 실패한 파싱을 분석하기 위해. 파싱 에러 포인터를 반환합니다. 이해하려면 몇 글자 뒤를 확인해야 할 수도 있습니다. cJSON_Parse()가 0을 반환할 때 정의됩니다. cJSON_Parse()가 성공하면 0을 반환합니다. */
CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);

/* 항목 타입을 확인하고 값을 반환 */
CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item);

/* 항목의 타입을 확인하는 함수들 */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);

/* 적절한 타입의 cJSON 항목을 생성하는 함수들 */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);
CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);
CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);
/* 원시 JSON */
CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);

/* valuestring이 문자열을 참조하도록 하는 문자열 생성, 이는 cJSON_Delete에 의해 해제되지 않습니다 */
CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string);
/* 객체/배열을 생성하여 그 요소를 참조만 하도록 하여 cJSON_Delete에 의해 해제되지 않도록 합니다 */
CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child);
CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child);

/* count 항목의 배열을 생성하는 유틸리티 함수들.
 * 숫자 배열의 요소 수보다 count가 크면 배열 접근이 범위를 벗어나므로 사용할 수 없습니다.*/
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count);

/* 지정된 배열/객체에 항목을 추가합니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
/* 문자열이 확실히 상수임(예: 리터럴 또는 거의 상수)이고 cJSON 객체를 통해 확실히 유지될 경우 사용합니다.
 * 주의: 이 함수 사용 시 항상 (item->type & cJSON_StringIsConst)가 0인지 확인한 후 `item->string`에 쓰기 작업을 수행해야 합니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
/* 배열/객체에 항목의 참조를 추가합니다. 기존 cJSON을 새로운 cJSON에 추가하지만 기존 cJSON을 손상시키지 않으려는 경우 사용합니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* 배열/객체에서 항목을 제거/분리합니다. */
CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

/* 배열 항목 업데이트 */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* 기존 항목을 오른쪽으로 이동시킵니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);

/* cJSON 항목 복제 */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
/* 
Duplicate는 전달된 cJSON 항목과 동일한 새 cJSON 항목을 새로운 메모리에 생성합니다. 
(recurse가 0이 아니면 항목에 연결된 모든 자식을 복제합니다.)
Duplicate의 반환값은 항상 item->next와 item->prev 포인터가 0입니다.
*/
/* 두 개의 cJSON 항목을 재귀적으로 비교하여 동일한지 확인합니다. a 또는 b가 NULL이거나 유효하지 않은 경우 동일하지 않은 것으로 간주됩니다.
 * case_sensitive는 객체 키가 대소문자를 구분하는지(1) 또는 구분하지 않는지(0)를 결정합니다. */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);

/* 문자열을 최소화하여 공백 문자(예: ' ', '\t', '\r', '\n')를 제거합니다.
 * 입력 포인터 json은 읽기 전용 주소 영역(예: 문자열 상수)을 가리키면 안 되며, 읽기 가능하고 쓰기 가능한 주소 영역을 가리켜야 합니다. */
CJSON_PUBLIC(void) cJSON_Minify(char *json);

/* 객체에 항목을 생성하고 동시에 추가하는 도우미 함수들.
 * 추가된 항목을 반환하거나 실패 시 NULL을 반환합니다. */
CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);

/* 정수 값을 할당할 때, valuedouble에도 전파되어야 합니다. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* cJSON_SetNumberValue 매크로를 위한 도우미 함수 */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* cJSON_String 객체의 valuestring을 변경합니다. 객체의 타입이 cJSON_String일 때만 효과가 있습니다. */
CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* 객체가 불리언 타입이 아니면 아무 작업도 하지 않고 cJSON_Invalid을 반환합니다. 그렇지 않으면 새로운 타입을 반환합니다. */
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False|cJSON_True))) ? \
    (object)->type=((object)->type &(~(cJSON_False|cJSON_True)))|((boolValue)?cJSON_True:cJSON_False) : \
    cJSON_Invalid\
)

/* 배열 또는 객체를 순회하기 위한 매크로 */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* cJSON_InitHooks로 설정된 malloc/free 함수를 사용하여 객체를 malloc/free */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif