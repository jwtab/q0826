
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

#include <unistd.h>
#include <dirent.h>

/*
    第一个问题.
*/
// Input 目录名称
// Output： 文件列表清单
class Findupdate {

private:
    vector<string> n_list;

public:
    void cleanList()
    {
        n_list.clear();
    }

    void getList(vector<string> & new_list)
    {
        new_list = n_list;
    }

    int listLength()
    {
        return n_list.size();
    }

public:
    bool checkFile(string fileName) 
    {
        DIR * dir = nullptr;
        int count = 1;

        cleanList();

        dir = opendir(fileName.c_str());
        if(nullptr == dir)
        {
            return false;
        }

        while(true)
        {
            dirent * ent = readdir(dir);
            if(nullptr == ent)
            {
                break;
            }

            if(0 == strcmp(ent->d_name,".") ||
                0 == strcmp(ent->d_name,".."))
            {
                continue;
            }
            else if(DT_REG == ent->d_type)
            {
                string file_stp_name = ent->d_name;
                size_t last_dot_pos = file_stp_name.find_last_of(".");
                if(string::npos == last_dot_pos)
                {
                    continue;
                }

                if(0 != file_stp_name.substr(last_dot_pos).compare(".stp"))
                {
                    continue;
                }

                string file_path_old = fileName;
                string file_path_new = "";
                char index_str[4] = {0};

                snprintf(index_str,4,"-%02d",count);

                file_path_old = file_path_old + "/";
                file_path_old = file_path_old + file_stp_name;

                file_path_new = file_path_old;
                file_path_new = file_path_new + index_str;
                
                while(1)
                {
                    if(0 == access(file_path_new.c_str(),F_OK))
                    {
                        count++;

                        snprintf(index_str,4,"-%02d",count);
                        
                        file_path_new = file_path_old;
                        file_path_new = file_path_new + index_str;
                    }
                    else
                    {
                        break;
                    }
                }

                if(0 == rename(file_path_old.c_str(),file_path_new.c_str()))
                {
                    count++;

                    n_list.push_back(file_path_new);
                }
                else
                {
                    //errno
                }
            }
        }

        closedir(dir);
        dir = nullptr;

        return listLength();
    }
};

/*
    第二个问题.
*/
///Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution
{
public:
    void printList(ListNode *head)
    {
        ListNode * node = head;

        cout<<"List vals:"<<endl
            <<"-------------------------------------------------"<<endl;

        while(nullptr != node)
        {
            cout<<" "<<node->val;
            node = node->next;
        }

        cout<<endl<<"-------------------------------------------------"<<endl;
    }

    void deleteList(ListNode *head)
    {
        while(nullptr != head)
        {
            ListNode *temp = head;

            head = head->next;

            delete temp;
            temp = nullptr;
        }
    }

    ListNode * createList(vector<int> & nums)
    {
        if(nums.size() <= 0)
        {
            return nullptr;
        }

        ListNode *head = new ListNode(nums[0]);
        if(nullptr != head)
        {
            ListNode * cur = head;
            for (size_t i = 1; i < nums.size(); i++)
            {
                cur->next = new ListNode(nums[i]);
                if(nullptr != cur->next)
                {
                    cur = cur->next;
                }
            }
        }

        return head;
    }

    /*
        
        slow ..... fast

            slow*****fast == 10
                [slow ... fast]  **tag

                    slow = fast->next
                        [slow ... fast] == 10?
    */
    ListNode* remove_(ListNode *head,bool &deleted)
    {
        unordered_map<ListNode*,int> to_delete_nodes;

		ListNode* dummy = new ListNode(1);
		dummy->next = head;
    
		//从slow的val开始累乘，如果结果是10，即直接让slow = fast->next.
		ListNode *slow = head;
        ListNode *fast = nullptr;
        
        while(slow)
        {
            int mut = slow->val;
            bool found = false;

            fast = slow->next;

            while(fast)
            {
                if(0 == to_delete_nodes.count(fast))
                {
                    mut = mut * fast->val;
                    if(abs(mut) > 10)
                    {
                        break;
                    }
                    else if(10 == mut)
                    {
                        found = true;

                        cout<<"TO_DELETE : "<<endl;
                        //保存要删除的节点.
                        ListNode *temp = slow;
                        while(temp != fast)
                        {
                            cout<<temp->val<<endl;
                            to_delete_nodes[temp] = temp->val;
                            temp = temp->next;
                        }

                        cout<<temp->val<<endl;
                        to_delete_nodes[temp] = temp->val;

                        cout<<"TO_DELETE_END "<<endl<<endl;

                        break;
                    }
                }

                fast = fast->next;
            }

            if(found)
            {
                deleted = true;
                slow = fast->next;
            }
            else
            {
                slow = slow->next;
            }
        }
        
        //真正删除.
        ListNode *pre = dummy;
        while(nullptr != pre->next)
        {
            ListNode *temp = pre->next;

            if(to_delete_nodes.count(temp) > 0 || 10 == temp->val)
            {
                to_delete_nodes.erase(temp);

                pre->next = temp->next;

                delete temp;
                temp = nullptr;
            }
            else
            {
                pre = pre->next;
            }
        }

        head = dummy->next;

        delete dummy;
        dummy = nullptr;

		return head;
    }

    ListNode* removeSublists(ListNode* head)
    {
        bool deleted = false;
        ListNode *new_head = head;

        int delete_count = 0;
        while(true)
        {
            ListNode *temp = new_head;
            deleted = false;

            new_head = remove_(temp,deleted);
            if(!deleted || nullptr == new_head)
            {
                break;
            }
            else
            {
                delete_count++;
            }
        }

        cout<<"Delete count_rings "<<delete_count<<endl;

        return new_head;
    }
};

int main(int argc,char **argv)
{
    /*
    Findupdate find;
    vector<string> new_file_list;

    if(find.checkFile("/home/dxlogin/test"))
    {
        find.getList(new_file_list);

        for(auto i:new_file_list)
        {
            cout<<"NEW_FILE : "<<i<<endl;
        }
    }
    */

    /* [10,2,2,5]*/
    vector<int> nums = {-2,5,-1,-10,1,-1,10,2,2,2,5,5};

    Solution s;

    ListNode *head = s.createList(nums);
    s.printList(head);

    cout<<endl;
    
    ListNode *new_list = s.removeSublists(head);
    s.printList(new_list);

    s.deleteList(new_list);
    
    return 0;
}
