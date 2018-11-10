// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_REQUIRES_STATEMENT_H
#define AST_REQUIRES_STATEMENT_H

#include "ASTExecutionStatement.h"

class ASTRequiresStatement : public ASTExecutionStatement
{
  protected:
    string resource;
    Expression *quantity;
    Expression *count;
    Expression *size;
    string tofrom;
    vector<ASTTrait*> traits;
  public:
    ASTRequiresStatement(string r, Expression *count, Expression *size, string tf, ParseVector<ASTTrait*> a)
        : resource(r), count(count), size(size), tofrom(tf), traits(a.begin(), a.end())
    {
        if (size)
            quantity = new BinaryExpr("*", count->Cloned(), size->Cloned());
        else
            quantity = count->Cloned();
    }
    virtual ~ASTRequiresStatement()
    {
        delete quantity;
        delete count;
        delete size;
        for (unsigned int i=0; i<traits.size(); ++i)
        {
            delete traits[i];
        }
    }


    const string &GetResource() const
    {
        return resource;
    }
    const Expression *GetQuantity() const
    {
        return quantity;
    }
    const Expression *GetSize() const
    {
        return size;
    }
    const Expression *GetCount() const
    {
        return count;
    }
    const string &GetToFrom() const
    {
        return tofrom;
    }
    unsigned int GetNumTraits() const
    {
        return traits.size();
    }
    const ASTTrait *GetTrait(int i) const
    {
        return traits[i];
    }
    const vector<ASTTrait*> &GetTraits() const
    {
        return traits;
    }
    /**
     * Returns true if this ASTRequiresStatement contains the input resource with traits.
     * Input resourcewithtraits : the input resource with traits in the form of resource_trait1_trait2.. (e.g., loads_random)
     * Return true if this ASTRequiresStatement contains the input resource with traits.
     */
    const bool ContainResource(string resourcewithtraits) const
    {
    	bool contains = true;
    	std::vector<std::string> elems;
    	std::stringstream ss;
    	ss.str(resourcewithtraits);
    	string item;
    	while (std::getline(ss, item, '_')) {
    		elems.push_back(item);
    	}
    	if( elems.size() == 0 ) {
    		return false;
    	} else if( elems[0] == "bytes" ) {
    		if( (resource != "loads") && (resource != "stores") ) {
    			return false;
    		}
    	} else if( elems[0] != resource ) {
    		return false;
    	}
    	int checked = elems.size()-1;
    	bool elems_contains_sp = false;
    	bool traits_contains_sp = false;
    	for (unsigned m=1; m<elems.size(); m++) {
    		if( !contains ) {
    			break;
    		}
    		string etrait = elems[m];
    		for (unsigned t = 0; t < traits.size(); ++t)
    		{
    			string ttrait = traits[t]->GetName();
    			if( etrait == ttrait ) {
    				if (ttrait == "stride")
    				{
    					try
    					{
    						double stride = traits[t]->GetValue()->Evaluate();
    						if (stride <= 1.0) {
    							contains = false;
    							break;
    						} else {
    							checked--;
    						}
    					}
    					catch (...)
    					{
    						// ignore errors from evaluating the trait quantity
    					}
    				} else {
    					checked--;
    				}
    			} else if( etrait == "sp" ) {
    				elems_contains_sp = true;
    				if( ttrait == "dp" ) {
    					contains = false;
    					break;
    				} else if( ttrait == "sp" ) {
    					traits_contains_sp = true;
    				}
    			}
    		}
    	}
    	if( contains ) {
    		if( checked == 1 ) {
    			if( !elems_contains_sp || traits_contains_sp ) {
    				contains = false;
    			}
    		}
    	}
#if 0
    	cout << "resource to test = " + resourcewithtraits + "\n";
    	cout << "traits to exam = ";
    		for (unsigned t = 0; t < traits.size(); ++t)
    		{
    			cout << " " + traits[t]->GetName();
    		}
    	if( contains ) {
    		cout << "\nresult = true\n";
    	} else {
    		cout << "\nresult = false\n";
    	}
#endif
    	return contains;
    }


    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Requires "<<resource << " of quantity: " << endl;
        quantity->Print(out, indent+1);
        // if we don't have a size, then quantity==count, so don't
        // try to separate the two when printing
        if (size)
        {
            out << Indent(indent+1) << "Where count is: " << endl;
            count->Print(out, indent+2);
            out << Indent(indent+1) << "And size is: " << endl;
            size->Print(out, indent+2);
        }
        if (tofrom != "")
            out << Indent(indent+1) << "To/From: "<<tofrom << endl;
        for (unsigned int i=0; i<traits.size(); i++)
        {
            traits[i]->Print(out, indent+1);
        }
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        ///\todo: do '[count] of size [size]' if size is not NULL
        out << Indent(indent) <<resource << " [" << quantity->GetText(Expression::ASPEN) << "]";
        if (tofrom != "")
        {
            ///\todo: guessing to/from here.
            if (resource=="loads")
                out << " from " << tofrom;
            else
                out << " to " << tofrom;
        }
        if (traits.size() > 0)
            out << " as ";
        for (unsigned int i=0; i<traits.size(); i++)
        {
            traits[i]->Export(out, indent);
            if (i+1<traits.size())
                out << ", ";
        }
        out << endl;
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;

        if (quantity)
            quantity->Visit(visitor);
        if (count)
            count->Visit(visitor);
        if (size)
            size->Visit(visitor);
        for (unsigned int i=0; i<traits.size(); ++i)
            traits[i]->Visit(visitor);
    }
};

#endif
