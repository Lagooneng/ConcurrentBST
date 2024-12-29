/*
*	DKU Operating System Lab
*	    Lab2 (Concurrent BST)
*	    Student id : 32204236
*	    Student name : 정현우
*/
#include "bst.h"

#ifndef BST_IMPI_H
#define BST_IMPI_H

/**
 * @brief BST without lock
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class BST : public DefaultBST {
	private:
		// 멤버 변수 추가 선언 가능
		Node *root = NULL;
		int count = 0;	// insert 함수의 호출 횟수 카운트
	public:
		// 멤버 함수 추가 선언 가능
		void insert(int key, int value) override;
		int lookup(int key) override;
		void remove(int key) override;
		void traversal(KVC* arr) override;
};

/**
 * @brief BST with coarse-grained lock
 * BST 전체를 critical section으로 가정하여, 하나의 lock으로 이를 관리한다.
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class CoarseBST : public DefaultBST {
	private:
		// 멤버 변수 추가 선언 가능
		// pthread_mutex_t mutex_lock;
		Node *root = NULL;
		pthread_mutex_t mutex_lock;
		int count = 0;	// insert 함수의 호출 횟수 카운트
	public:
		// 멤버 함수 추가 선언 가능
		CoarseBST();	// 생성자에서 락 init
		void insert(int key, int value) override;
		int lookup(int key) override;
		void remove(int key) override;
		void traversal(KVC* arr) override;
};

/// @brief FineBST는 FineNode를 정의하여 사용하길 권장한다.
struct FineNode : public Node {
    // 멤버 변수 추가 가능
	FineNode *fineLeft;
	FineNode *fineRight;
	pthread_mutex_t mutex_lock;
};

/**
 * @brief BST with fine-grained lock
 * BST 내부의 critical section을 Node 단위로 lock으로 관리한다.
 * Node를 상속받은 FineNode를 정의하여 사용하길 권장한다.
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class FineBST : public DefaultBST {
	private:
		// 멤버 변수 추가 선언 가능
		FineNode *root = NULL;
		int count = 0;	// insert 함수의 호출 횟수 카운트
		pthread_mutex_t mutex_lock_root;
		pthread_mutex_t mutex_lock_count;
		pthread_mutex_t mutex_lock_arr_index;
		pthread_mutex_t mutex_lock_traverse_arr_;
	public:
		// 멤버 함수 추가 선언 가능
		FineBST();	// FineNode 내부 락은 insert시 init하고 그 외 락은 생성자에서 초기화 
		void insert(int key, int value) override;
		int lookup(int key) override;
		void remove(int key) override;
		void traversal(KVC* arr) override;
};
#endif
