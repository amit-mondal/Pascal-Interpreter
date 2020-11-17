


def print_pas_proc(suffix):
    print(f'''
procedure r{suffix}() -> real;
begin
    return r{suffix-1}();
end;''')

def pas_proc():
    print('''
    program Main;
    var v :  real;

    procedure r0() -> real;
    begin
       return 5;
    end;''')

    for i in range(1, 10000):
        print_proc(i)
        print()

    print('''
    begin
       v := r0();
       dump(v);
    end.''')
