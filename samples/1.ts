function printf(msg: string, value: int): void;

// function printf(msg: string, value: double): void;
// function printf(msg: string, value: string): void;

class A {
    x: int = 10;
    y: double;
    z: boolean;
    w: string = "hello";

    sayHello(): void {
        this.x = this.x * 100;
        printf("Hello World, %d\n", this.x);
    }

    getValue(): int {
        return 100;
    }

}

function main(): int {

    let b: A = new A();
    b.x = 20;

    try {
        b.sayHello();
        let x: int = b.getValue();
        b.x = x;
        b.sayHello();
    } catch (e: int) {

    }


    return 0;

}