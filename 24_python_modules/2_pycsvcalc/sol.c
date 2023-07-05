#define PY_SSIZE_T_CLEAN

#include <Python.h>

const char* py_code = "import sys\n"
                      "table_data = sys.stdin.read()\n"
                      "cells = [row.split(';') for row in table_data.split('\\n')]\n"
                      "def isfloat(num):\n"
                      "    try:\n"
                      "        float(num)\n"
                      "        return True\n"
                      "    except ValueError:\n"
                      "        return False\n"
                      "def calc(formula : str):\n"
                      "    if (len(formula) > 0 and formula[0] == '='):\n"
                      "        formula = formula[1:]\n"
                      "        i = 0\n"
                      "        while i < len(formula) - 1:\n"
                      "            if (formula[i] >= 'A' and formula[i] <= 'Z' and formula[i+1].isdigit):\n"
                      "                if (cells[int(formula[i+1])-1][ord(formula[i])-ord('A')][0] == '='):\n"
                      "                    formula = formula.replace(formula[i:i+2], f\"({cells[int(formula[i+1])-1][ord(formula[i])-ord('A')][1:]})\")\n"
                      "                    i = 0\n"
                      "                else:\n"
                      "                    formula = formula.replace(formula[i:i+2], f\"({cells[int(formula[i+1])-1][ord(formula[i])-ord('A')]})\")\n"
                      "                    i = 0\n"
                      "            else:\n"
                      "                i+=1\n"
                      "        returned = eval(formula)\n"
                      "        if not (str(returned).isdigit() or isfloat(str(returned))):\n"
                      "            returned = f\"\\\"{returned}\\\"\"\n"
                      "        return str(returned)\n"
                      "    else:\n"
                      "        return formula\n"
                      "cells = [list(map(calc, row)) for row in cells]\n"
                      "table_data = '\\n'.join([';'.join(row) for row in cells])\n"
                      "print(table_data)";

int main(int argc, char* argv[]) {
  Py_Initialize();
  PyRun_SimpleString(py_code);
  Py_Finalize();
  return 0;
}
