/*
*	DKU Operating System Lab
*	    Lab2 (Concurrent BST)
*	    Student id : 32204236
*	    Student name : 정현우
*/
#include "bst_impl.h"

void BST::insert(int key, int value){
	// Todo 
	Node *node = new Node();

	node->key = key;
	node->value = value;
	node->upd_cnt = 0;
	if( root == NULL ) { 
		root = node;
		count++;
		return;
	}
	
	count++;

	Node *nowNode = root;

	// 삽입 위치 탐색 
	while(true) {
		if( nowNode->key == key ) {	// 이미 키가 있는 경우
			nowNode->value += value;
			nowNode->upd_cnt++;
			count--;
			delete node;
			break;
		}
		else if( nowNode->key < key ) {		// 현재 노드보다 키가 더 큰 경우
			if( nowNode->right == NULL ) {	// 우측 자식이 없으면 삽입
				nowNode->right = node;
				break;
			}

			nowNode = nowNode->right;		// 우측 자식이 있으면 우측으로 진행
		}
		else {
			if( nowNode->left == NULL ) {	// 현재 노드보다 키가 더 작은 경우
				nowNode->left = node;		// 좌측 자식이 없으면 삽입
				break;
			}

			nowNode = nowNode->left;		// 좌측 자식이 있으면 좌측으로 진행 
		}
	}
	
}

int BST::lookup(int key) {
	// Todo
	Node *node = root;

	while(true) {
		if( node == NULL ) break;	// 찾는 키 없음

		if( node->key < key ) {
			node = node->right;
		}
		else if( node->key > key ) {
			node = node->left;
		}
		else {						// 탐색 성공
			return node->value;
		}
	}

	return 0;
} 

void BST::remove(int key){
	// Todo
	Node *parentNode = NULL;
	Node *nowNode = root;
	Node *sucParentNode;
	Node *sucNode;
	
	// 삭제 대상 위치 탐색 
	while ( nowNode != NULL )
	{
		if( nowNode->key == key ) break;
		parentNode = nowNode;

		if ( nowNode->key < key )
		{
			nowNode = nowNode->right;
		}
		else {
			nowNode = nowNode->left;
		}
	}

	if( nowNode == NULL ) return;

	// remove, 3 cases
	// case 1: no child
	if( nowNode->left == NULL && nowNode->right == NULL ) {
		if( nowNode == root ) {
			delete root;
			this->root = NULL;
			return;
		}
		if( parentNode != NULL ) {	// 부모 노드와 링크를 끊음
			if( parentNode->left == nowNode ) {
				parentNode->left = NULL;
			}
			else {
				parentNode->right = NULL;
			}
		}
		else {
			delete root;
			this->root = NULL;
		}
	}	// case 2a: right child
	else if( nowNode->left == NULL ) {
		if( nowNode == root ) {
			this->root = nowNode->right;
			delete nowNode;
			return;
		}

		if( parentNode != NULL ) { // 부모 노드와의 연결을 자식 노드와의 연결로 붙이고 자신은 트리에서 나옴
			if( parentNode->left == nowNode ) {
				parentNode->left = nowNode->right;
			}
			else {
				parentNode->right = nowNode->right;
			}
		}
		else {
			root = nowNode->right;
		}

		delete nowNode;
	}	// case 2b: left child
	else if( nowNode->right == NULL ) {
		if( nowNode == root ) {
			this->root = nowNode->left;
			delete nowNode;
			return;
		}

		if( parentNode != NULL ) {	// 부모 노드와의 연결을 자식 노드와의 연결로 붙이고 자신은 트리에서 나옴
			if( parentNode->left == nowNode ) {
				parentNode->left = nowNode->left;
			}
			else {
				parentNode->right = nowNode->left;
			}
		}
		else {
			root = nowNode->left;
		}

		delete nowNode;
	}	// case 3: double child
	else {
		sucParentNode = nowNode;
		sucNode = nowNode->left;

		// 계승 노드 탐색
		while ( sucNode->right != NULL )
		{
			sucParentNode = sucNode;
			sucNode = sucNode->right;
		}

		if( sucParentNode->left == sucNode ) {
			sucParentNode->left = sucNode->left;
		}
		else {
			sucParentNode->right = sucNode->left;
		}

		// 링크를 건드리지 않고 값을 변경하여 처리
		nowNode->key = sucNode->key;
		nowNode->value = sucNode->value;
		nowNode->upd_cnt = sucNode->upd_cnt;

		delete sucNode;
	}
	
}

void BST::traversal(KVC* traverse_arr) {
	// Todo
	if( count == 0 ) return;
	arr_index_ = 0;
	
	// 트리 크기 만큼의 배열을 스택으로 사용 
	Node *stack = new Node[count + 1];
	Node *nowNode = root;
	int idx = 0;

	// 중위 순회 
	while ( nowNode != NULL || idx > 0 )
	{
		while ( nowNode != NULL )
		{
			stack[idx].key = nowNode->key;
			stack[idx].value = nowNode->value;
			stack[idx].upd_cnt = nowNode->upd_cnt;
			stack[idx].left = nowNode->left;
			stack[idx].right = nowNode->right;
			idx++;

			nowNode = nowNode->left;
		}

		nowNode = &(stack[idx - 1]);
		idx--;

		traverse_arr[arr_index_].key = nowNode->key;
		traverse_arr[arr_index_].value = nowNode->value;
		traverse_arr[arr_index_].upd_cnt = nowNode->upd_cnt;
		arr_index_++;

		nowNode = nowNode->right;
	}
	
	delete[] stack;
}


CoarseBST::CoarseBST() {
	pthread_mutex_init(&mutex_lock, NULL);
}

void CoarseBST::insert(int key, int value){
	// Todo
	Node *node = new Node();
	node->key = key;
	node->value = value;
	node->upd_cnt = 0;

	// 트리에 대해 락 시작
	pthread_mutex_lock(&mutex_lock);
	if( root == NULL ) {
		root = node;
		count++;
		pthread_mutex_unlock(&mutex_lock);	// 트리가 빈 경우 빠르게 종료 가능
		return;
	}
	count++;
	Node *nowNode = root;
	
	while(true) {
		if( nowNode->key == key ) {
			nowNode->value += value;
			nowNode->upd_cnt++;
			delete node;
			break;
		}
		else if( nowNode->key < key ) {
			if( nowNode->right == NULL ) {
				nowNode->right = node;
				break;
			}

			nowNode = nowNode->right;
		}
		else {
			if( nowNode->left == NULL ) {
				nowNode->left = node;
				break;
			}

			nowNode = nowNode->left;
		}
	}
	pthread_mutex_unlock(&mutex_lock);	// 삽입 종료 : 언락
}

int CoarseBST::lookup(int key) {
	// Todo
	// 락 시작
	pthread_mutex_lock(&mutex_lock);
	Node *node = root;

	while(true) {
		if( node == NULL ) break;

		if( node->key < key ) {
			node = node->right;
		}
		else if( node->key > key ) {
			node = node->left;
		}
		else {
			pthread_mutex_unlock(&mutex_lock);
			return node->value;
		}
	}
	pthread_mutex_unlock(&mutex_lock);	// 탐색 종료 : 언락
	return 0;
} 

void CoarseBST::remove(int key){
	// Todo
	Node *sucParentNode;
	Node *sucNode;
	Node *parentNode = NULL;
	
	// 트리 접근 시작: 락
	pthread_mutex_lock(&mutex_lock);

	Node *nowNode = root;
	
	while ( nowNode != NULL )
	{
		if( nowNode->key == key ) break;
		parentNode = nowNode;

		if ( nowNode->key < key )
		{
			nowNode = nowNode->right;
		}
		else {
			nowNode = nowNode->left;
		}
	}

	if( nowNode == NULL )  {
		pthread_mutex_unlock(&mutex_lock);
		return;
	}
	
	// case 1: no child
	if( nowNode->left == NULL && nowNode->right == NULL ) {
		if( nowNode == root ) {
			delete root;
			this->root = NULL;
			pthread_mutex_unlock(&mutex_lock);	// case1일 경우, 루트가 삭제 대상일 때의 언락
			return;
		}
		if( parentNode != NULL ) {
			if( parentNode->left == nowNode ) {
				parentNode->left = NULL;
			}
			else {
				parentNode->right = NULL;
			}
		}
		else {
			delete root;
			this->root = NULL;
		}
	}	// case 2a: right child
	else if( nowNode->left == NULL ) {
		if( nowNode == root ) {
			this->root = nowNode->right;
			delete nowNode;
			pthread_mutex_unlock(&mutex_lock);	// case2a일 경우, 루트가 삭제 대상일 때의 언락
			return;
		}

		if( parentNode != NULL ) {
			if( parentNode->left == nowNode ) {
				parentNode->left = nowNode->right;
			}
			else {
				parentNode->right = nowNode->right;
			}
		}
		else {
			root = nowNode->right;
		}

		delete nowNode;
	}	// case 2b: left child
	else if( nowNode->right == NULL ) {
		if( nowNode == root ) {
			this->root = nowNode->left;
			delete nowNode;
			pthread_mutex_unlock(&mutex_lock);	// // case2b일 경우, 루트가 삭제 대상일 때의 언락
			return;
		}

		if( parentNode != NULL ) {
			if( parentNode->left == nowNode ) {
				parentNode->left = nowNode->left;
			}
			else {
				parentNode->right = nowNode->left;
			}
		}
		else {
			root = nowNode->left;
		}

		delete nowNode;
	}	// case 3: double child
	else {
		sucParentNode = nowNode;
		sucNode = nowNode->left;

		while ( sucNode->right != NULL )
		{
			sucParentNode = sucNode;
			sucNode = sucNode->right;
		}

		if( sucParentNode->left == sucNode ) {
			sucParentNode->left = sucNode->left;
		}
		else {
			sucParentNode->right = sucNode->left;
		}

		nowNode->key = sucNode->key;
		nowNode->value = sucNode->value;
		nowNode->upd_cnt = sucNode->upd_cnt;

		delete sucNode;
	}
	pthread_mutex_unlock(&mutex_lock);	// 예외 처리(case1, case2a, case2b에서 삭제 대상이 루트인 경우)가 되지 않는 일반적인 경우의 언락
}

void CoarseBST::traversal(KVC* traverse_arr) {
    // Todo
	// 순회 시작: 락
	pthread_mutex_lock(&mutex_lock);
	if( count == 0 ) {
		pthread_mutex_unlock(&mutex_lock);
		return;
	}
	arr_index_ = 0;
	
	Node *stack = new Node[count + 1];
	Node *nowNode = root;
	int idx = 0;

	while ( nowNode != NULL || idx > 0 )
	{
		while ( nowNode != NULL )
		{
			stack[idx].key = nowNode->key;
			stack[idx].value = nowNode->value;
			stack[idx].upd_cnt = nowNode->upd_cnt;
			stack[idx].left = nowNode->left;
			stack[idx].right = nowNode->right;
			idx++;

			nowNode = nowNode->left;
		}

		nowNode = &(stack[idx - 1]);
		idx--;

		traverse_arr[arr_index_].key = nowNode->key;
		traverse_arr[arr_index_].value = nowNode->value;
		traverse_arr[arr_index_].upd_cnt = nowNode->upd_cnt;
		arr_index_++;

		nowNode = nowNode->right;
	}
	pthread_mutex_unlock(&mutex_lock);	// 순회 종료: 언락
	delete[] stack;
}

FineBST::FineBST() {
	pthread_mutex_init(&mutex_lock_count, NULL);
	pthread_mutex_init(&mutex_lock_root, NULL);
	pthread_mutex_init(&mutex_lock_arr_index, NULL);
	pthread_mutex_init(&mutex_lock_traverse_arr_, NULL);
}

void FineBST::insert(int key, int value){
	// Todo
	FineNode *node = new FineNode();
	// 삽입할 노드의 락 init
	pthread_mutex_init(&(node->mutex_lock), NULL);

	// 락 하기 전 밖으로 빼도 되는 건 미리 처리 
	node->key = key;
	node->value = value;
	node->upd_cnt = 0;
	// 루트에 락을 걸면서 시작 
	pthread_mutex_lock(&mutex_lock_root);
	if( root == NULL ) {	// 루트에 값이 없는 경우(빈 트리)
		pthread_mutex_lock(&mutex_lock_count);
		root = node;
		count++;
		pthread_mutex_unlock(&mutex_lock_count);
		pthread_mutex_unlock(&mutex_lock_root);
		return;
	}
	pthread_mutex_unlock(&mutex_lock_root);

	pthread_mutex_lock(&mutex_lock_count);
	count++;
	pthread_mutex_unlock(&mutex_lock_count);

	FineNode *nowNode = root;

	while(true) {
		pthread_mutex_lock(&(nowNode->mutex_lock));	// Fine lock
		if( nowNode->key == key ) {
			nowNode->value += value;
			nowNode->upd_cnt++;
			delete node;
			break;
		}
		else if( nowNode->key < key ) {
			if( nowNode->fineRight == NULL ) {
				nowNode->fineRight = node;
				break;
			}

			FineNode *temp = nowNode;
			nowNode = nowNode->fineRight;
			pthread_mutex_unlock(&(temp->mutex_lock));	// 지나간 노드는 놔줘도 됨
		}
		else {
			if( nowNode->fineLeft == NULL ) {
				nowNode->fineLeft = node;
				break;
			}

			FineNode *temp = nowNode;
			nowNode = nowNode->fineLeft;
			pthread_mutex_unlock(&(temp->mutex_lock));	// 지나간 노드는 놔줘도 됨
		}
	}
	pthread_mutex_unlock(&(nowNode->mutex_lock));	// 삽입 종료 
}

int FineBST::lookup(int key) {
	// Todo
	int v = 0;
	
	pthread_mutex_lock(&(mutex_lock_root));
	FineNode *node = root;
	if( root == NULL ) {
		pthread_mutex_unlock(&mutex_lock_root);
	}
	pthread_mutex_unlock(&mutex_lock_root);

	while(true) {
		if( node == NULL ) {
			break;
		}
		
		pthread_mutex_lock(&(node->mutex_lock));	// Fine lock
		FineNode *temp = node;
		if( node->key < key ) {
			node = node->fineRight;
		}
		else if( node->key > key ) {
			node = node->fineLeft;
		}
		else {
			int v = node->value;
			pthread_mutex_unlock(&(node->mutex_lock));	// 탐색 성공
			break;
		}
		pthread_mutex_unlock(&(temp->mutex_lock));	// 지나간 노드는 놔줌
	}

	return v;
} 

void FineBST::remove(int key){
	// Todo
	
	FineNode *parentNode = NULL;
	pthread_mutex_lock(&mutex_lock_root);
	FineNode *nowNode = root;
	pthread_mutex_unlock(&mutex_lock_root);
	FineNode *sucParentNode;
	FineNode *sucNode;
	
	if(nowNode == NULL) return;
	int flag = 0;
	while ( true )
	{
		if(nowNode == NULL) break;
		
		pthread_mutex_lock( &(nowNode->mutex_lock) );
		
		if( nowNode->key == key ) {
			break;
		}
		if( flag > 0 ) {
			pthread_mutex_unlock(&(parentNode->mutex_lock));	// 부모의 부모 노드가 되는 타이밍엔 놔줘도 됨
		}
		parentNode = nowNode;
		flag = 1;
		if ( nowNode->key < key )
		{
			nowNode = nowNode->fineRight;
		}
		else {
			nowNode = nowNode->fineLeft;
		}
	}
	
	if( nowNode == NULL ) {	// 탐색 실패
		if( parentNode != NULL ) pthread_mutex_unlock(&(parentNode->mutex_lock));
		return;
	}
	
	// case 1: no child
	if( nowNode->fineLeft == NULL && nowNode->fineRight == NULL ) {
		if( nowNode == root ) {
			if( parentNode != NULL ) pthread_mutex_unlock(&(parentNode->mutex_lock));
			pthread_mutex_unlock(&(nowNode->mutex_lock));
			pthread_mutex_lock(&mutex_lock_root);
			delete root;
			this->root = NULL;
			pthread_mutex_unlock(&(mutex_lock_root));
			return;
		}
		if( parentNode != NULL ) {
			if( parentNode->fineLeft == nowNode ) {
				parentNode->fineLeft = NULL;
			}
			else {
				parentNode->fineRight = NULL;
			}
			// nowNode 는 이제 언링크됨
			pthread_mutex_unlock(&(parentNode->mutex_lock));
			pthread_mutex_unlock(&(nowNode->mutex_lock));
			delete nowNode;
		}
		else {
			pthread_mutex_unlock(&(nowNode->mutex_lock));
			pthread_mutex_lock(&mutex_lock_root);
			delete root;
			this->root = NULL;
			pthread_mutex_unlock(&mutex_lock_root);
		}
	}	// case 2a: right child
	else if( nowNode->fineLeft == NULL ) {
		if( nowNode == root ) {
			pthread_mutex_lock(&mutex_lock_root);
			this->root = nowNode->fineRight;
			pthread_mutex_unlock(&mutex_lock_root);
			pthread_mutex_unlock(&(nowNode->mutex_lock));
			delete nowNode;
			return;
		}

		if( parentNode != NULL ) {
			if( parentNode->fineLeft == nowNode ) {
				parentNode->fineLeft = nowNode->fineRight;
			}
			else {
				parentNode->fineRight = nowNode->fineRight;
			}
		}
		else {
			pthread_mutex_lock(&mutex_lock_root);
			root = nowNode->fineRight;
			pthread_mutex_unlock(&mutex_lock_root);
		}
		if( parentNode != NULL ) pthread_mutex_unlock(&(parentNode->mutex_lock));
		pthread_mutex_unlock(&(nowNode->mutex_lock));
		delete nowNode;
	}	// case 2b: left child
	else if( nowNode->fineRight == NULL ) {
		if( nowNode == root ) {
			pthread_mutex_lock(&mutex_lock_root);
			this->root = nowNode->fineLeft;
			pthread_mutex_unlock(&mutex_lock_root);
			pthread_mutex_unlock(&(nowNode->mutex_lock));
			delete nowNode;
			return;
		}

		if( parentNode != NULL ) {
			if( parentNode->fineLeft == nowNode ) {
				parentNode->fineLeft = nowNode->fineLeft;
			}
			else {
				parentNode->fineRight = nowNode->fineLeft;
			}
		}
		else {
			pthread_mutex_lock(&mutex_lock_root);
			root = nowNode->fineLeft;
			pthread_mutex_unlock(&mutex_lock_root);
		}
		if( parentNode != NULL ) pthread_mutex_unlock(&(parentNode->mutex_lock));
		pthread_mutex_unlock(&(nowNode->mutex_lock));
		delete nowNode;
	}	// case 3: double child
	else {
		if( parentNode != NULL ) pthread_mutex_unlock(&(parentNode->mutex_lock));

		sucParentNode = nowNode;
		sucNode = nowNode->fineLeft;

		pthread_mutex_lock(&(sucNode->mutex_lock));
		flag = 0;
		while ( sucNode->fineRight != NULL )
		{
			if( sucNode != nowNode->fineLeft ) pthread_mutex_lock(&(sucNode->mutex_lock));
			if( flag > 0 && sucParentNode != nowNode ) {
				pthread_mutex_unlock(&(sucParentNode->mutex_lock));
			}
			flag = 1;
			sucParentNode = sucNode;
			sucNode = sucNode->fineRight;
		}

		if( sucParentNode->fineLeft == sucNode ) {
			sucParentNode->fineLeft = sucNode->fineLeft;
		}
		else {
			sucParentNode->fineRight = sucNode->fineLeft;
		}

		if( sucParentNode != nowNode ) pthread_mutex_unlock(&(sucParentNode->mutex_lock));
		pthread_mutex_unlock(&(sucNode->mutex_lock));
		
		nowNode->key = sucNode->key;
		nowNode->value = sucNode->value;
		nowNode->upd_cnt = sucNode->upd_cnt;

		
		pthread_mutex_unlock(&(nowNode->mutex_lock));
		delete sucNode;
	}
}

void FineBST::traversal(KVC* traverse_arr) {
    // Todo
	if( count == 0 ) return;
	arr_index_ = 0;
	
	FineNode *stack = new FineNode[count + 1];
	pthread_mutex_lock(&mutex_lock_root);
	FineNode *nowNode = root;
	pthread_mutex_unlock(&mutex_lock_root);
	pthread_mutex_t *mutex_temp;
	int idx = 0;

	while ( nowNode != NULL || idx > 0 )
	{
		while ( nowNode != NULL )
		{
			pthread_mutex_lock(&(nowNode->mutex_lock));
			mutex_temp = &(nowNode->mutex_lock);

			stack[idx].key = nowNode->key;
			stack[idx].value = nowNode->value;
			stack[idx].upd_cnt = nowNode->upd_cnt;
			stack[idx].fineLeft = nowNode->fineLeft;
			stack[idx].fineRight = nowNode->fineRight;
			idx++;

			nowNode = nowNode->fineLeft;
			pthread_mutex_unlock(mutex_temp);
		}

		nowNode = &(stack[idx - 1]);
		idx--;

		pthread_mutex_lock(&(nowNode->mutex_lock));
		traverse_arr[arr_index_].key = nowNode->key;
		traverse_arr[arr_index_].value = nowNode->value;
		traverse_arr[arr_index_].upd_cnt = nowNode->upd_cnt;
		pthread_mutex_lock(&mutex_lock_arr_index);
		arr_index_++;
		pthread_mutex_unlock(&mutex_lock_arr_index);

		mutex_temp = &(nowNode->mutex_lock);
		nowNode = nowNode->fineRight;
		pthread_mutex_unlock(mutex_temp);
	}
	
	delete[] stack;
}
