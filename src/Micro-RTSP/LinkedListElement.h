#pragma once

#include "platglue.h"
#include <stdio.h>

class LinkedListElement
{
public:
    LinkedListElement* m_Next;
    LinkedListElement* m_Prev;
    
    LinkedListElement(void)
    {
        m_Next = this;
        m_Prev = this;
        printf("LinkedListElement (%p)->(%p)->(%p)\n", m_Prev, this, m_Next);
    }
    
    int NotEmpty(void)
    {
        return (m_Next != this);
    }
    
    LinkedListElement(LinkedListElement* linkedList)
    {
        // add to the end of list
        m_Prev = linkedList->m_Prev;
        linkedList->m_Prev = this;
        m_Prev->m_Next = this;
        m_Next = linkedList;
        printf("LinkedListElement (%p)->(%p)->(%p)\n", m_Prev, this, m_Next);
    }
    
    ~LinkedListElement()
    {
       printf("~LinkedListElement(%p)->(%p)->(%p)\n", m_Prev, this, m_Next);
       if (m_Next)
           m_Next->m_Prev = m_Prev;
       if (m_Prev)
           m_Prev->m_Next = m_Next;
       printf("~LinkedListElement after: (%p)->(%p)", m_Prev, m_Prev->m_Next);
    }
};
