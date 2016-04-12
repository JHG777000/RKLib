/*
 Copyright (c) 2016 Jacob Gordon. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: RKMem.c
//RKMem, provides some nice macros and a funtion to make dealling with memory a little easier in C.
//As well as a double linked-list implementation, RKList.
//And a dictionary type, RKStore.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RKMem.h"

 #define RKS_MAX_LETTER_NUM 94

 typedef struct RKStore_letter_s { RKList_node node ; struct RKStore_letter_s* next_alphabet ; } RKStore_letter ;

 struct RKStore_s { RKStore_letter* dictionary ; RKList items ; } ;

 struct RKList_node_s { struct RKList_node_s* before ; struct RKList_node_s* after ; void* data ; } ;

 struct RKList_object_s { int num_of_nodes ; RKList_node first ; RKList_node last ; } ;

 void* RKMem_Realloc_Func(void* data, size_t newsize, size_t oldsize, int NULLonError0No1Yes) {
    
    void* newdata ;
    
    newdata = malloc(newsize) ;
    
    if ( newdata != NULL ) {
        
        memcpy(newdata, data, oldsize) ;
        
        free(data) ;
        
        return newdata ;
        
    } else {
        
        if ( NULLonError0No1Yes ) {
            
            free(data) ;
            
            return NULL ;
            
        } else {
            
            return data ;
        }
    }
}

RKList RKList_NewList( void ) {
    
    RKList newlist = RKMem_NewMemOfType(RKList_object) ;
    
    newlist->num_of_nodes = 0 ;
    
    newlist->first = NULL ;
    
    newlist->last = NULL ;
    
    return newlist ;

}

RKList RKList_NewListFromArray( void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size ) {
    
    RKList newlist = RKList_NewList() ;
    
    RKList_CopyToListFromArray(newlist, array, GetDataFromArrayFunc, size) ;
    
    return newlist ;
}

RKList_node RKList_AddToList( RKList list, void* data ) {
    
    if ( list->num_of_nodes == 0 ) {
        
        list->first = RKMem_NewMemOfType(RKList_node_object) ;
        
        list->first->before = NULL ;
        
        list->first->after = NULL ;
        
        list->last = list->first ;
        
    } else {
        
        list->last->after = RKMem_NewMemOfType(RKList_node_object) ;
        
        list->last->after->before = list->last ;
        
        list->last->after->after = NULL ;
        
        list->last = list->last->after ;
    }
    
    list->last->data = data ;
    
    list->num_of_nodes++ ;
    
    return list->last ;
}

RKList_node RKList_AddNodeToList( RKList_node node, RKList list ) {
    
    return RKList_AddToList(list, node->data) ;
}

void RKList_NodeSwap( RKList list, RKList_node node_a, RKList_node node_b ) {
    
    RKList_node Before = node_a->before ;
    
    RKList_node After = node_a->after ;
    
    node_a->before = node_b->before ;
    
    node_a->after = node_b->after ;
    
    if ( node_a->before != NULL ) node_a->before->after = node_a ;
    
    if ( node_a->after != NULL ) node_a->after->before = node_a ;
    
    if ( node_a->before == NULL ) list->first = node_a ;
    
    if ( node_a->after == NULL ) list->last = node_a ;
    
    node_b->before = Before ;
    
    node_b->after = After ;
    
    if ( node_b->before != NULL ) node_b->before->after = node_b ;
    
    if ( node_b->after != NULL ) node_b->after->before = node_b ;
    
    if ( node_b->before == NULL ) list->first = node_b ;
    
    if ( node_b->after == NULL ) list->last = node_b ;
    
}

void RKList_DataSwap( RKList_node node_a, RKList_node node_b ) {
    
    void* data = node_a->data ;
    
    node_a->data = node_b->data ;
    
    node_b->data = data ;
}

void RKList_InsertNodeAToNodeB( RKList list, RKList_node node_a, RKList_node node_b ) {
    
    if ( node_a->before != NULL ) node_a->before->after = node_a->after ;
    
    if ( node_a->after != NULL ) node_a->after->before = node_a->before ;
    
    if ( node_a->before == NULL ) list->first = node_a->after ;
    
    if ( node_a->after == NULL ) list->last = node_a->before ;
    
    node_a->before = node_b->before ;
    
    node_a->after = node_b ;
    
    node_b->before->after = node_a ;
    
    node_b->before = node_a ;
}

RKList_node RKList_MoveNodeFromListToList( RKList_node node, RKList list_a, RKList list_b ) {
    
    RKList_node new_node = RKList_AddNodeToList( node, list_b ) ;
    
    RKList_DeleteNode(list_a, node) ;
    
    return new_node ;
}

void RKList_CopyList(RKList list_a, RKList list_b) {
    
    RKList_node node = RKList_GetFirstNode(list_a) ;
    
    while ( node != NULL ) {
        
        RKList_AddNodeToList( node, list_b ) ;
        
        node = RKList_GetNextNode(node) ;
    }
}

void RKList_CopyToListFromArray(RKList list, void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size) {
    
    void* data = NULL ;
    
    int i = 0 ;
    
    while ( i < size ) {
        
        data = GetDataFromArrayFunc(array,i) ;
        
        RKList_AddToList(list, data) ;
        
        i++ ;
    }

}

void RKList_SetData(RKList_node node, void* data) {
    
    node->data = data ;
}

void* RKList_GetData(RKList_node node) {
    
    return node->data ;
}

RKList_node RKList_GetNextNode(RKList_node node) {
    
    return node->after ;
}

RKList_node RKList_GetPreviousNode(RKList_node node) {
    
    return node->before ;
}

RKList_node RKList_GetFirstNode(RKList list) {
    
    return list->first ;
}

RKList_node RKList_GetLastNode(RKList list) {
    
    return list->last ;
}

int RKList_GetNumOfNodes(RKList list) {
    
    return list->num_of_nodes ;
}

void RKList_DeleteNode( RKList list, RKList_node node ) {
    
    if ( node == NULL ) return ;
    
    if ( ( node->before == NULL ) || ( node->after == NULL ) ) {
    
     if ( ( node->before == NULL ) && ( node->after == NULL ) ) {
            
         list->first = NULL ;
            
         list->last = NULL ;
     }
        
     if ( ( node->before == NULL ) && ( node->after != NULL ) ) {
        
         list->first = node->after ;
        
         list->first->before = NULL ;
     }
    
     if ( ( node->before != NULL ) && ( node->after == NULL ) ) {
        
         list->last = node->before ;
        
         list->last->after = NULL ;
     }
    
    }
    
    if ( ( node->before != NULL ) && ( node->after != NULL ) ) {
        
        node->before->after = node->after ;
        
        node->after->before = node->before ;
    }
    
    list->num_of_nodes-- ;
    
    free(node) ;
}

RKList_node RKList_DeleteNodeAndReturnNextNode( RKList list, RKList_node node ) {
    
    RKList_node next_node = RKList_GetNextNode(node) ;
    
    RKList_DeleteNode(list, node) ;
    
    return next_node ;
}

void RKList_DeleteList( RKList list ) {
    
    while ( list->first != NULL ) {
        
        RKList_DeleteNode(list,list->first) ;
    }
    
    free(list) ;
}

void RKList_DeleteAllNodesInList( RKList list ) {
    
    while ( list->first != NULL ) {
        
        RKList_DeleteNode(list,list->first) ;
    }
}

RKList_node RKList_GetNode( RKList list, int index ) {
    
    int i = 1 ;
    
    RKList_node node = list->first ;
    
    while ( i < index ) {
        
        if ( node == NULL ) return NULL ;
        
        node = node->after ;
        
        i++ ;
    }
    
    return node ;
}

void RKList_DeleteNodeWithIndex( RKList list, int index ) {
    
    RKList_node node = RKList_GetNode(list, index) ;
    
    RKList_DeleteNode(list, node) ;
}

void RKList_IterateListWith( RKMemIteratorFuncType Iterator, RKList list ) {
    
    RKList_node node = list->first ;
    
    void* data = NULL ;
    
    while ( node != NULL ) {
        
        data = RKList_GetData(node) ;
        
        Iterator(data) ;
        
        node = RKList_GetNextNode(node) ;
    }
}

static int RKS_LookUpCharID( char c ) {
    
    int value = (c - 32) + 1 ;
    
    if ( value <= 0 ) value = 0 ;
    
    if ( value > RKS_MAX_LETTER_NUM + 1) value = 0 ;
    
    return value ;
}

static RKStore_letter* RKS_NewAlphabet( void ) {
    
    RKStore_letter* rks_alphabet = NULL ;
    
    int size = RKS_MAX_LETTER_NUM + 1 ;
    
    rks_alphabet = RKMem_CArray(size, RKStore_letter) ;
    
    int i = 0 ;
    
    while ( i < size ) {
        
        rks_alphabet[i].node = NULL ;
        
        rks_alphabet[i].next_alphabet = NULL ;
        
        i++ ;
    }
    
    return rks_alphabet ;
    
}

static RKList_node RKS_LookUpStoreNode( RKStore Store, int* buffer, int size, RKList_node node ) {
    
    RKStore_letter* current_alphabet = NULL ;
    
    int i = 0 ;
    
    int value = 0 ;
    
    if ( Store->dictionary == NULL ) {
        
        Store->dictionary = RKS_NewAlphabet() ;
    }
    
    current_alphabet = Store->dictionary ;
    
    while ( i < size ) {
        
        value = buffer[i] ;
        
        if ( value != 0 ) {
            
            if ( i == ( size - 1 ) ) {
                
                if ( node == NULL ) {
                    
                    return current_alphabet[value].node ;
                    
                } else {
                    
                    current_alphabet[value].node = node ;
                    
                    return current_alphabet[value].node ;
                    
                }
                
            } else {
                
                if ( current_alphabet[value].next_alphabet == NULL ) current_alphabet[value].next_alphabet = RKS_NewAlphabet() ;
                
                current_alphabet = current_alphabet[value].next_alphabet ;
                
            }
            
        }
        
        i++ ;
    }
    
    return NULL ;
}

static RKList_node RKS_GetSetStoreNode( RKStore Store, const char* label, int size, RKList_node node ) {
    
    int buffer[100] ;
    
    int i = 0 ;
    
    if ( size > 100 ) size = 100 ;
    
    while (i < size) {
        
        buffer[i] = RKS_LookUpCharID( label[i] ) ;
        
        i++ ;
    }
    
    return RKS_LookUpStoreNode( Store, buffer, size, node ) ;
    
}

static RKList_node RKS_GetSetNode( RKStore Store, const char* string, int min, int max, RKList_node node ) {
    
    char* label = NULL ;
    
    label = &(((char*)string)[min]) ;
    
    return RKS_GetSetStoreNode(Store,label,max,node) ;
}

static RKList_node RKS_GetSetNodeWithString( RKStore Store, const char* string, RKList_node node ) {
    
    int strilen = 0 ;
    
    strilen = (int) strlen( string ) ;
    
    return RKS_GetSetNode(Store,string,0,strilen,node) ;
}

RKStore RKStore_NewStore( void ) {
    
    RKStore Store = RKMem_NewMemOfType(struct RKStore_s) ;
    
    Store->dictionary = NULL ;
    
    Store->items = NULL ;
    
    return Store ;
}

static RKList_node RKS_AddItem( RKStore Store, void* item ) {
    
    if ( Store->items == NULL ) Store->items = RKList_NewList() ;
    
    return RKList_AddToList(Store->items, item) ;
}

static int RKS_StoreItem( RKStore Store, void* item, const char* label ) {
    
    RKList_node node = RKS_GetSetNodeWithString(Store, label, NULL) ;
    
    if ( node == NULL ) return 0 ;
    
    RKList_SetData(node, item) ;
    
    return 1 ;
}

int RKStore_AddItem( RKStore Store, void* item, const char* label ) {
    
    if ( !(RKStore_ItemExists(Store, label)) ) {
    
        RKList_node node = RKS_AddItem(Store, item) ;
    
        node = RKS_GetSetNodeWithString(Store, label, node) ;
        
        if ( node == NULL ) return 0 ;
        
    } else {
        
        return RKS_StoreItem(Store, item, label) ;
    }
    
    return 1 ;
}

int RKStore_RemoveItem( RKStore Store, const char* label ) {
    
    RKList_node node = RKS_GetSetNodeWithString(Store, label, NULL) ;
    
    if ( node == NULL ) return 0 ;
    
    RKList_SetData(node, NULL) ;
    
    return 1 ;
}

void* RKStore_GetItem( RKStore Store, const char* label ) {
    
    RKList_node node = RKS_GetSetNodeWithString(Store, label, NULL) ;
    
    if ( node == NULL ) return NULL ;
    
    return RKList_GetData(node) ;
}

int RKStore_ItemExists( RKStore Store, const char* label ) {
    
    RKList_node node = RKS_GetSetNodeWithString(Store, label, NULL) ;
    
    if ( node != NULL ) return 1 ;
    
    return 0 ;
}

void RKStore_IterateStoreWith( RKMemIteratorFuncType Iterator, RKStore Store ) {
    
    if (Store->items != NULL) RKList_IterateListWith(Iterator, Store->items) ;
}

static void RKS_DestroyAlphabet( RKStore_letter* alphabet ) {
    
    int size = RKS_MAX_LETTER_NUM + 1 ;
    
    int i = 0 ;
    
    while ( i < size ) {
        
        if (alphabet[i].next_alphabet != NULL)  {
            
            RKS_DestroyAlphabet(alphabet[i].next_alphabet) ;
            
            alphabet[i].next_alphabet = NULL ;
        }
        
        i++ ;
    }

    free(alphabet) ;
}

void RKStore_DestroyStore( RKStore Store ) {
    
    if (Store->dictionary != NULL) RKS_DestroyAlphabet(Store->dictionary) ;
    
    if (Store->items != NULL) RKList_DeleteList(Store->items) ;
    
    free(Store) ;
}
